#include "../inc/Supervisor.hpp"


int	findFdMax(fd_set all_sockets) {
	int	max = 0;

	for (int fd = 0; fd < FD_SETSIZE; ++fd) {
		if (FD_ISSET(fd, fdset) && fd > max)
			max = fd;
	}
	return max;
}

Supervisor::~Supervisor(void){
	return;
}

Supervisor::Supervisor(const Supervisor& src){
	if (this != &src)
		*this = src;
}

Supervisor::Supervisor(void){
	FD_ZERO(&(this->_all_sockets));
    FD_ZERO(&(this->_read_fds));
	FD_ZERO(&(this->_excep_fds));
	FD_ZERO(&(this->_write_fds));
	this->_timer.tv_sec = 2;
    this->_timer.tv_usec = 0;
}

void	Supervisor::fdSetAdd(unsigned int socket_fd){
	FD_SET(socket_fd, &(this->_all_sockets));
	FD_SET(socket_fd, &(this->_read_fds));
	FD_SET(socket_fd, &(this->_write_fds));
	FD_SET(socket_fd, &(this->_excep_fds));
	this->_fd_max = findFdMax(this->all_sockets);
}

void	Supervisor::fdSetRemove(unsigned int socket_fd){
	//close(socket_fd);
	FD_CLR(socket_fd, &(this->_all_sockets));
	FD_CLR(socket_fd, &(this->_read_fds));
	FD_CLR(socket_fd, &(this->_write_fds));
	FD_CLR(socket_fd, &(this->_excep_fds));
	this->_fd_max = findFdMax(this->all_sockets);
}

/*int		Supervisor::mapFindSocketType(int socket_fd) const {
	return this->_fd_map[socket_fd];
}

void	Supervisor::mapAddElement(int socket_fd, int is_server_socket){
	this->_fd_map[socket_fd] = is_server_socket;
	return;
}

void	Supervisor::mapRemoveElement(int socket_fd){
	this->_fd_map.erase(socket_fd);
	return;
}*/

void	Supervisor::addServer(ServerConfig server_config){
	Server new_server;

	new_server.launch();
	fdSetAdd(new_server.getSocket());
	this->_fd_map[new_server.getSocket()] = 1;
	this->_servers_map[new_server.getSocket()] = new_server;
	return;
}

//remove socket not server 
void	Supervisor::removeServer(unsigned int fd){
	fdSetRemove(fd);
	this->_servers_map.erase(fd);
	mapRemoveElement(fd);
	return;
}

void Supervisor::acceptNewConnection(int server_socket){
    int client_socket;

    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return ;
    }
    FD_SET(client_socket, all_sockets);
	this->_clients_map[client_socket] = server_socket;
	this->_fd_max = findFdMax(this->all_sockets);
	this->_fd_map[client_socket] = 0;
	this->_servers_map[server_socket]->addClient(client_socket, server_socket);
    printf("[Server] Accepted new connection on client socket %d.\n", client_socket);
    //check path otherwise return index.html
    if (send(client_socket, msg_to_send, strlen(msg_to_send), 0) == -1) {
        fprintf(stderr, "[Server] Send error to client %d: %s\n", client_socket, strerror(errno));
    }
}

void	Supervisor::manageOperations(void){
	while (1) {
        if (select(this->_fd_max + 1, &(this->_read_fds), &(this->_write_fds), &(this->_excep_fds), &(this->_timer))) {
            fprintf(stderr, "[Server] Select error: %s\n", strerror(errno));
            exit(1);
        }
        printf("[Server] Waiting...\n");
		for (int fd = 0; fd <= this->_fd_max; i++) {
			if (FD_ISSET(i, &(this->_read_fds)) != 0) {
				if (this->_fd_map[fd]) {
					// La socket est une socket serveur qui écoute le port->ajouter un client
					acceptNewConnection(fd);
				}
				else {
					// La socket est une socket client, on va la lire/parser/ajouter les info au client du serveur correspondant
					readRequestFromClient(i, &(this->all_sockets), fd_max, server_socket);
				}
			}
			if (FD_ISSET(i, &(this->_write_fds)) != 0) {
				if (this->_fd_map[fd]) {
					//error server socket 
				}
				else {
					// La socket client est prete a recevoit une reponse->recuperer information du client->send response
					writeResponseToClient(fd);
					
				}
			}
			if (FD_ISSET(i, &(this->_excep_fds)) != 0){
				//error
			}
		}
	}
	return;
}

void Supervisor::readRequestFromClient(int client_socket){
    char buffer[BUFSIZ];
    int bytes_read;
    int status;

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(client_socket, buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("[] Client socket closed connection.\n");
        }
        else {
            fprintf("[Server] Recv error: \n",);
        }
        fdSetRemove(client_socket);
    }
    else {
        // Louis -> parsing 
		int server_socket = this->_clients_map[client_socket];
		Server &server = this->_servers_map[server_socket]
        server.setClient(client_socket, buffer);
    }
}

void	Supervisor::writeResponseToClient(int client_socket){
	int server_socket = this->_clients_map[client_socket];
	Server server* = this->_servers_map[server_socket];
	Client client = server.getClient(client_socket);
	//Response response = buildResponse(client);

	//recup client avec sa socket -> examiner sa requete->renvoyer la reponse adequate->msg_to_send = content + http_response class
	/*send(client_socket, msg_to_send, strlen(msg_to_send), 0);
	if (status == -1) {
		fprintf(stderr, "[Server] Send error to client fd %d: %s\n", j, strerror(errno));
	}*/

}

