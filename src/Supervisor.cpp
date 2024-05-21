#include "../inc/Supervisor.hpp"

int	findFdMax(fd_set all_sockets) {
	int	max = 0;

	for (int fd = 0; fd < FD_SETSIZE; ++fd) {
		if (FD_ISSET(fd, &all_sockets) && fd > max)
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
	this->_fd_max = findFdMax(this->_all_sockets);
}

/*void	Supervisor::fdSetAdd(int socket_fd){
	FD_SET(socket_fd, &(this->_all_sockets));
	FD_SET(socket_fd, &(this->_read_fds));
	FD_SET(socket_fd, &(this->_write_fds));
	this->_fd_max = findFdMax(this->_all_sockets);
}*/

void	Supervisor::fdSetRemove(int socket_fd){
	//close(socket_fd);
	FD_CLR(socket_fd, &(this->_all_sockets));
	FD_CLR(socket_fd, &(this->_read_fds));
	FD_CLR(socket_fd, &(this->_write_fds));
	this->_fd_max = findFdMax(this->_all_sockets);
}

int		Supervisor::isServer(int socket_fd) const{
	std::map<int, Server*>::const_iterator it;

	it = this->_servers_map.find(socket_fd);
	if (it == this->_servers_map.end())
		return 0;
	return 1;
}

void	Supervisor::addServer(ServerConfig server_config){
	Server 				new_server;
	std::vector<int>	new_server_sockets;
	new_server.createServer(server_config);
	new_server_sockets = new_server.getSockets();
	for (unsigned long i = 0; i < new_server_sockets.size(); i++){
		FD_SET(new_server_sockets[i], &(this->_all_sockets));
		FD_SET(new_server_sockets[i], &(this->_read_fds));
		this->_fd_max = findFdMax(this->_all_sockets);
		this->_servers_map[new_server_sockets[i]] = &new_server;
		std::cout << "[Server] Server socket " << new_server_sockets[i] << " added to the supervisor" << std::endl;
	}
	return;
}

void	Supervisor::removeClients(int server_socket){
	for (std::map<int, Client>::iterator it = this->_clients_map.begin(); it != this->_clients_map.end(); ++it){
		if (it->second.getServerSocket() == server_socket){
			fdSetRemove(it->first);
			close(it->first);
			this->_clients_map.erase(it);
		}
	}
}

void	Supervisor::closeClient(int client_socket){
	std::map<int, Client>::iterator it = this->_clients_map.find(client_socket);
	this->_clients_map.erase(it);
	close(client_socket);
}

//remove server_socket -> manque remove clients + s the server really destrozyed?
void	Supervisor::removeServer(int server_socket){
	Server								*server = this->_servers_map[server_socket];
	std::vector<int>					server_sockets = server->getSockets();
	std::map<int, Server*>::iterator	it;

	for (unsigned long i = 0; i < server_sockets.size(); i++){
		fdSetRemove(server_sockets[i]);
		it = this->_servers_map.find(server_sockets[i]);
		this->_servers_map.erase(it);
		removeClients(server_socket);
		close(server_sockets[i]);
	}
	this->_fd_max = findFdMax(this->_all_sockets);
	return;
}

void Supervisor::acceptNewConnection(int server_socket){
    int 	client_socket;
	Client	new_client;

    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return ;
    }
    FD_SET(client_socket, &(this->_read_fds));
	FD_SET(client_socket, &(this->_all_sockets));
	this->_clients_map[client_socket] = new_client;
	this->_fd_max = findFdMax(this->_all_sockets);
    printf("[Server] Accepted new connection on client socket %d.\n", client_socket);
}

void	Supervisor::manageOperations(void){
	while (1) {
		this->_read_fds = this->_all_sockets;
		this->_write_fds = this->_all_sockets;
		//this->_excep_fds = this->_all_sockets;
        if (select(this->_fd_max + 1, &(this->_read_fds), &(this->_write_fds), NULL, &(this->_timer)) == -1) {
            fprintf(stderr, "[Server] Select error: %s\n", strerror(errno));
            exit(1);
        }
        printf("[Server] Waiting...\n");
		for (int fd = 0; fd <= this->_fd_max; fd++) {
			if (FD_ISSET(fd, &(this->_read_fds)) != 0) {
				if (isServer(fd)) {
					if (DEBUG)
						std::cout << "[Server] A connection with a new client is made" << std::endl;
					acceptNewConnection(fd);
				}
				else {
					if (DEBUG)
						std::cout << "[Client:" << fd << "] A request has been sent" << std::endl;
					readRequestFromClient(fd);
				}
			}
			if (FD_ISSET(fd, &(this->_write_fds)) != 0) {
				if (isServer(fd)) {
					std::cout << "[ERROR] Une socket server est ouverte pour lecture" << std::endl;
				}
				else {
					if (DEBUG)
						std::cout << "[Client:" << fd << "] A client is ready to receive a response" << std::endl;
					writeResponseToClient(fd);
					
				}
			}
			/*if (FD_ISSET(fd, &(this->_excep_fds)) != 0){
				std::cout << "[ERROR] An fd has been placed in the exception set by select" << std::endl;
			}*/
		}
	}
	return;
}

void Supervisor::readRequestFromClient(int client_socket){
    char buffer[BUFSIZ];
    int bytes_read;

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(client_socket, &buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
           std::cout << "[] Client socket closed connection." << std::endl;
        }
        else {
            std::cout << "[Server] Recv error:" << std::endl;;
        }
		FD_CLR(client_socket, &(this->_read_fds));
		closeClient(client_socket);
		this->_fd_max = findFdMax(this->_all_sockets);
    }
    else {
		FD_SET(client_socket, &(this->_write_fds));
		FD_CLR(client_socket, &(this->_read_fds));
		this->_clients_map[client_socket].setData(buffer);
    }
}

void	Supervisor::writeResponseToClient(int client_socket){
	Client client = this->_clients_map[client_socket];
	Server *server = this->_servers_map[client.getServerSocket()];
	Response response(client, server->getServerConfig());

	FD_SET(client_socket, &(this->_write_fds));
	FD_CLR(client_socket, &(this->_write_fds));
	if (send(client_socket, response.getFinalReply().c_str(), response.getFinalReply().length(), MSG_DONTWAIT)){
		fprintf(stderr, "[Server] Send error to client fd %d: %s\n", client.getServerSocket(), strerror(errno));
	}
	std::cout << "successfully sent response" << std::endl; 

}

void	Supervisor::buildServers(Config configuration){
	std::vector<ServerConfig> servers;

	servers = configuration.getServers();
	for (unsigned long i = 0; i < servers.size(); i++){
		addServer(servers[i]);
	}
	return;
}

