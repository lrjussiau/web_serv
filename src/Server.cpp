#include "../inc/Server.hpp"

//split launch socket en deux?

Server::Server(void){
	return;
}

Server::~Server(void){
	return;
}

Server::Server(const Server& src){
	if (this != &src)
		*this = src;
}

Server&	Server::operator=(const Server& src){
	this->_sockets = src.getSockets();
	return *this;
}

/*				getters						*/

std::vector<int>	Server::getSockets(void) const{
	return this->_sockets;
}

ServerConfig	Server::getServerConfig(void) const{
	return this->_server_config;
}


/*				launch server				*/

void	Server::createServer(ServerConfig server_config){
	this->_server_config = server_config;
	for (size_t i = 0; i < server_config.listen_ports.size(); i++) {
		if (server_config.is_ipv4){
			try{
				launchSocket(server_config.listen_ports[i], server_config.server_name, server_config.is_ipv4);
			}
			catch (std::exception &e){
        	std::cerr << RED << e.what() << std::endl;
			}
		}
    }
	return;
}

int Server::launchSocket(uint32_t port, std::string ip, bool IPv4) {
	struct sockaddr_in sa;
	int					server_socket;
	int					opt = 1;

    memset(&sa, 0, sizeof sa);
	if (IPv4){
		inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);
    	sa.sin_family = AF_INET;
	}
	else{
		inet_pton(AF_INET6, ip.c_str(), &sa.sin_addr);
		sa.sin_family = AF_INET6;
	}
    sa.sin_port = htons(port);
	server_socket = socket(sa.sin_family, SOCK_STREAM, 0);
    this->_sockets.push_back(server_socket);
    if (server_socket == -1) {
        throw ServerSocketError();
    }
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    if (bind(server_socket, (struct sockaddr *)&sa, sizeof sa)) {
		fprintf(stderr, "[Server] Binding error: %s\n", strerror(errno));
    	//throw ServerBindingError();
	}
    if (listen(server_socket, 10)) {
        throw ServerListeningError();
    }
	if (DEBUG)
		std::cout << GRN << "[Server] Created server socket fd: " << server_socket << " at ip:port : " << ip << port << std::endl;
	return (0);
}
