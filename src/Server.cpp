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
        	std::cerr << RED << e.what() << RST << std::endl;
			}
		}
    }
	return;
}

std::string	resolveDomainToIp(std::string domain_name){
	struct	addrinfo hints, *res, *p;
    char 	ip_str[INET_ADDRSTRLEN];
	void 	*addr;

	std::cout << "ipstr: " << std::endl;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(domain_name.c_str(), nullptr, &hints, &res)) {
		//gai_strerror(status)
        std::cerr << "getaddrinfo error: " << std::endl;
        exit(-1);
    }
	for (p = res; p != nullptr; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
			break;
		}
	}
	inet_ntop(p->ai_family, addr, ip_str, sizeof(ip_str));
	freeaddrinfo(res);

	return std::string(ip_str);
}

int Server::launchSocket(uint32_t port, std::string ip, bool domain_name) {
	struct sockaddr_in sa;
	int					server_socket;

    memset(&sa, 0, sizeof sa);
	if (!domain_name)
		ip = resolveDomainToIp(ip);
	std::cout << "ip: "<< ip << std::endl;
	inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);
	sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
	server_socket = socket(sa.sin_family, SOCK_STREAM, 0);
	setNonBlocking(server_socket, 1);
    this->_sockets.push_back(server_socket);
    if (server_socket == -1) {
        throw ServerSocketError();
    }
    if (bind(server_socket, (struct sockaddr *)&sa, sizeof sa)) {
		fprintf(stderr, "[Server] Binding error: %s\n", strerror(errno));
    	//throw ServerBindingError();
	}
    if (listen(server_socket, SOMAXCONN)) {
        throw ServerListeningError();
    }
	if (DEBUG)
		std::cout << GRN << "[Server] Created server socket fd: " << server_socket << " at ip:port : " << ip << ":" << port << RST << std::endl;
	return (0);
}
