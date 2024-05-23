#include "../inc/Supervisor.hpp"

/* a faire: gere les fd set proprement, function pour tout shutdown, rrearranger function/ordre*/

Supervisor::~Supervisor(void){
	std::cout << GRN <<  "Closing all connections" << RST << std::endl;
	for (std::map<int, Server*>::iterator it = this->_servers_map.begin(); it != this->_servers_map.end(); ++it){
		close(it->first);
	}
	for (std::map<int, Client>::iterator it = this->_clients_map.begin(); it != this->_clients_map.end(); ++it){
		close(it->first);
	}
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
	updateFdMax();
}

/*				manipulation des fds				*/

void	Supervisor::updateFdMax(void) {
	int	max = 0;

	for (int fd = 0; fd < FD_SETSIZE; ++fd) {
		if (FD_ISSET(fd, &(this->_all_sockets)) && fd > max)
			max = fd;
	}
	this->_fd_max = max;
}

void	Supervisor::fdSetRemove(int socket){
	//close(socket_fd);
	FD_CLR(socket, &(this->_all_sockets));
	FD_CLR(socket, &(this->_read_fds));
	FD_CLR(socket, &(this->_write_fds));
	updateFdMax();
}

/*				util				*/

int		Supervisor::isServer(int socket) const{
	std::map<int, Server*>::const_iterator it;

	it = this->_servers_map.find(socket);
	if (it == this->_servers_map.end())
		return 0;
	return 1;
}

/*				server creation/deletion				*/


void	Supervisor::runServers(Config configuration){
	std::vector<ServerConfig> servers_config = configuration.getServers();
	Server 				*new_server = NULL;
	std::vector<int>	new_server_sockets;

	for (std::vector<ServerConfig>::iterator server_cnf = servers_config.begin(); server_cnf != servers_config.end(); ++server_cnf){
		new_server = new Server();
		new_server->createServer(*server_cnf);
		new_server_sockets = new_server->getSockets();
		for (std::vector<int>::iterator server_sock = new_server_sockets.begin(); server_sock != new_server_sockets.end(); ++server_sock){
			FD_SET(*server_sock, &(this->_all_sockets));
			FD_SET(*server_sock, &(this->_read_fds));
			this->_servers_map[*server_sock] = new_server;
		}
	}
	updateFdMax();
	return;
}

void	Supervisor::closeServer(int server_socket){
	Server								*server = this->_servers_map[server_socket];
	std::vector<int>					server_sockets = server->getSockets();
	std::map<int, Server*>::iterator	it;

	for (unsigned long i = 0; i < server_sockets.size(); i++){
		fdSetRemove(server_sockets[i]);
		it = this->_servers_map.find(server_sockets[i]);
		//free(it->second);
		this->_servers_map.erase(it);
		removeClientsFromServer(server_socket);
		close(server_sockets[i]);
	}
	updateFdMax();
	return;
}

/*				client deletion				*/

void	Supervisor::removeClientsFromServer(int server_socket){
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

	FD_CLR(client_socket, &(this->_read_fds));
	this->_clients_map.erase(it);
	updateFdMax();
	close(client_socket);
}


void	Supervisor::manageOperations(void){
	while (1) {
		this->_read_fds = this->_all_sockets;
		//this->_write_fds = this->_all_sockets;
		//this->_excep_fds = this->_all_sockets;
        if (select(this->_fd_max + 1, &(this->_read_fds), &(this->_write_fds), NULL, &(this->_timer)) == -1) {
            fprintf(stderr, "[Server] Select error: %s\n", strerror(errno));
            exit(1);
        }
       	std::cout << YEL << "[Servers] Waiting..." << RST << std::endl;
		for (int fd = 0; fd <= this->_fd_max; fd++) {
			if (FD_ISSET(fd, &(this->_read_fds)) != 0) {
				if (isServer(fd)) {
					if (DEBUG)
						std::cout << GRN << "[Server "<< fd << "] A connection with a new client is made" << RST << std::endl;
					acceptNewConnection(fd);
				}
				else {
					if (DEBUG)
						std::cout << GRN << "[Client:" << fd << "] A request has been sent" << RST << std::endl;
					readRequestFromClient(fd);
				}
			}
			if (FD_ISSET(fd, &(this->_write_fds)) != 0) {
				if (isServer(fd)) {
					std::cout << RED << "[ERROR] Une socket server est ouverte pour lecture" << RST << std::endl;
				}
				else {
					if (DEBUG)
						std::cout << GRN <<"[Client:" << fd << "] A client is ready to receive a response" << RST <<std::endl;
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

void Supervisor::acceptNewConnection(int server_socket){
    int 	client_socket;

    client_socket = accept(server_socket, NULL, NULL);
	Client	new_client(server_socket, client_socket);
    if (client_socket == -1) {
        std::cout << RED << "[Server " << server_socket << "] Accept error" << RST << std::endl;
        return ;
    }
    FD_SET(client_socket, &(this->_read_fds));
	FD_SET(client_socket, &(this->_all_sockets));
	this->_clients_map[client_socket] = new_client;
	updateFdMax();
	std::cout << GRN << "[Server " << server_socket << "] Accepted new connection on client socket: " << client_socket << RST << std::endl;
}

void Supervisor::readRequestFromClient(int client_socket){
    char buffer[BUFSIZ];
    int bytes_read;

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(client_socket, &buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
           std::cout << GRN << "[Client " << client_socket << "] socket closed connection." << RST << std::endl;
        }
        else {
            std::cout << GRN << "[Server "<< this->_clients_map[client_socket].getServerSocket() << "] Recv error:" << RST << std::endl;;
        }
		closeClient(client_socket);
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

	FD_SET(client_socket, &(this->_read_fds));
	FD_CLR(client_socket, &(this->_write_fds));
	if (send(client_socket, response.getFinalReply().c_str(), response.getFinalReply().length(), MSG_DONTWAIT) == -1){
		std::cout << RED << "[Server "<< client.getServerSocket() << "] Send error to client fd: " << client.getSocket() << RST << std::endl;
		//fprintf(stderr, "[Server] Send error to client fd %d: %s\n", client.getServerSocket(), strerror(errno));
	}
	else
		std::cout << GRN << "successfully sent response" << RST <<  std::endl; 

}

