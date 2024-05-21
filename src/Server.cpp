#include "../inc/Server.hpp"


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

std::vector<int>	Server::getSockets(void) const{
	return this->_sockets;
}

void	Server::createServer(ServerConfig server_config){
	this->_server_config = server_config;
	for (size_t i = 0; i <= server_config.listen_ports.size(); i++) {
		launchSocket(server_config.listen_ports[i], server_config.server_name, server_config.is_ipv4);
		//std::cout << "hello" << std::endl;
		/*catch (const std::exception& e){
        	std::cerr << e.what() << std::endl;
		}*/
    }
	return;
}

ServerConfig	Server::getServerConfig(void) const{
	return this->_server_config;
}
//ipv4 a regler
int Server::launchSocket(uint32_t port, std::string ip, bool IPv4) {
	struct sockaddr_in sa;
	int					server_socket;

    memset(&sa, 0, sizeof sa);
	if (IPv4 == false){
		//std::cout << "Ipv4"<< std::endl;
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
    if (bind(server_socket, (struct sockaddr *)&sa, sizeof sa)) {
    	throw ServerBindingError();
	}
    if (listen(server_socket, 10)) {
        throw ServerListeningError();
    }
	if (DEBUG)
		std::cout << "[Server] Created server socket fd: " << server_socket << "at ip:port : " << ip << port << std::endl;
	return (0);
}
