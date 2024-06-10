#include "../inc/Supervisor.hpp"
#include "../inc/Server.hpp"

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

/*				util				*/

int		Supervisor::isServer(int socket) const{
	std::map<int, Server*>::const_iterator it;

	it = this->_servers_map.find(socket);
	if (it == this->_servers_map.end())
		return 0;
	return 1;
}

void		Supervisor::shutDown(void){
	std::cout << GRN <<  "Closing all connections" << RST << std::endl;
	for (std::map<int, Server*>::iterator it = this->_servers_map.begin(); it != this->_servers_map.end(); ++it){
		close(it->first);
	
	}
	for (std::map<int, Client*>::iterator it = this->_clients_map.begin(); it != this->_clients_map.end(); ++it){
		close(it->first);
	}
	exit(0);
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

void	Supervisor::closeClient(int client_socket){
	std::map<int, Client*>::iterator it = this->_clients_map.find(client_socket);

	close(client_socket);
    FD_CLR(client_socket, &this->_all_sockets);
    FD_CLR(client_socket, &this->_read_fds);
    FD_CLR(client_socket, &this->_write_fds);
	this->_clients_map.erase(it);
	if (it->second != NULL)
		delete it->second;
	updateFdMax();

}

void	Supervisor::manageOperations(void){
	while (1) {
		if (running != true)
			shutDown();
		fd_set read_fds = this->_read_fds;
		fd_set write_fds = this->_write_fds;
        if (select(this->_fd_max + 1, &read_fds, &write_fds, NULL, &(this->_timer)) == -1) {
			if (running != true)
				shutDown();
			else
            	std::cerr << RED << "Select error" << RST << std::endl;
        }
       	std::cout << YEL << "[Servers] Waiting..." << RST << std::endl;
		for (int fd = 0; fd <= this->_fd_max; fd++) {
			if (FD_ISSET(fd, &read_fds) != 0) {
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
			if (FD_ISSET(fd, &write_fds) != 0) {
				if (isServer(fd)) {
					std::cout << RED << "[ERROR] Une socket server est ouverte pour lecture" << RST << std::endl;
				}
				else {
					if (DEBUG)
						std::cout << GRN <<"[Client:" << fd << "] A client is ready to receive a response" << RST <<std::endl;
					writeResponseToClient(fd);
					
				}
			}
		}
	}
	return;
}

void Supervisor::acceptNewConnection(int server_socket){
    int 			client_socket;
	std::string		cookie =  generateSessionId();

    client_socket = accept(server_socket, NULL, NULL);
	setNonBlocking(client_socket, 0);
	Client	*new_client = new Client(server_socket, client_socket, cookie);
    if (client_socket == -1) {
        std::cout << RED << "[Server " << server_socket << "] Accept error" << RST << std::endl;
        return ;
    }
    FD_SET(client_socket, &(this->_read_fds));
	FD_SET(client_socket, &(this->_all_sockets));
	this->_clients_map[client_socket] = new_client;
	updateFdMax();
	std::cout << GRN << "[Server " << server_socket << "] Accepted new connection on client socket: " << client_socket << RST << std::endl;
	return;
}

void Supervisor::readRequestFromClient(int client_socket) {
    char buffer[1000000];
    int	bytes_read;
	bool	nothing_read = true;	
    std::ofstream request_file;
    std::string request_file_path = PATH_TO_REQUESTS;
	
    request_file.open(request_file_path.c_str(), std::ios::out | std::ios::app | std::ios::binary);
    if (!request_file.is_open()) {
        std::cerr << RED << "Failed to open request file" << RST << std::endl;
        closeClient(client_socket);
        return;
    }

    memset(&buffer, '\0', sizeof buffer);
    while ((bytes_read = recv(client_socket, &buffer, sizeof(buffer) - 1, 0)) > 0){
		nothing_read = false;
		request_file.write(buffer, bytes_read);
		if (!request_file) {
			std::cerr << RED <<  "Failed to write to request file" << RST << std::endl;
			return;
		}
		memset(&buffer, '\0', sizeof buffer);
    }
	if (nothing_read) {
		if (bytes_read == 0) {
			std::cout << GRN << "[Client " << client_socket << "] socket closed connection." << RST << std::endl;
			closeClient(client_socket);
		}
		else {
			std::cerr << ORG << "Error Caught : recv error" << RST << std::endl;
		}
	}
	else{
		request_file.close();
		this->_clients_map[client_socket]->setData(PATH_TO_REQUESTS);
		FD_SET(client_socket, &(this->_write_fds));
		FD_CLR(client_socket, &(this->_read_fds));
	}

	return;
}

void Supervisor::writeResponseToClient(int client_socket) {
    Client *client = this->_clients_map[client_socket];
    Server *server = this->_servers_map[client->getServerSocket()];
    ServerConfig conf = server->getServerConfig();
    Response	response(client, server->getServerConfig());
	ssize_t		bytes_sent;	

    FD_SET(client_socket, &(this->_read_fds));
    FD_CLR(client_socket, &(this->_write_fds));
	bytes_sent = send(client_socket, response.getFinalReply().c_str(), response.getFinalReply().length(), MSG_DONTWAIT);
    if (bytes_sent == -1) {
        closeClient(client_socket);
    } else if (bytes_sent == (int)response.getFinalReply().length()){
        std::cout << ORG << "successfully sent response" << RST << std::endl;
    }
	else{
		std::cout << ORG << "successfully sent part of response" <<  bytes_sent << RST << std::endl;
	}
    unlink(PATH_TO_REQUESTS);
    return;
}