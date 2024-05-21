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
	//this->_clients = src.getClients();
	return *this;
}

/*std::map<int, Client> Server::getClients(void) const{
	return this->_clients;
}*/

std::vector<int>	Server::getSockets(void) const{
	return this->_sockets;
}

void	Server::createServer(ServerConfig server_config){
	for (size_t i = 0; i <= server_config.listen_ports.size(); i++) {
		//std::cout << "  pppport: "<<server_config.listen_ports[i] << std::endl;
		launchSocket(server_config.listen_ports[i], server_config.server_name, server_config.is_ipv4);
		std::cout << "hello" << std::endl;
		/*catch (const std::exception& e){
        	std::cerr << e.what() << std::endl;
		}*/
    }
	return;
}

int Server::launchSocket(uint32_t port, std::string ip, bool IPv4) {
	struct sockaddr_in sa;
	int					server_socket;

	std::cout << "  port: "<< port << std::endl;
    // Préparaton de l'adresse et du port pour la socket de notre serveur
    memset(&sa, 0, sizeof sa);
	if (IPv4 == false){
		std::cout << "Ipv4"<< std::endl;
		inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);
    	sa.sin_family = AF_INET;
	}
	else{
		inet_pton(AF_INET6, ip.c_str(), &sa.sin_addr);
		sa.sin_family = AF_INET6;
	}
    sa.sin_port = htons(port);
    // Création de la socket
	server_socket = socket(sa.sin_family, SOCK_STREAM, 0);
	std::cout << "server socket"<< server_socket << "  port: "<< port << std::endl;
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
	printf("[Server] Created server socket fd: %d\n", server_socket);
	return (0);
}

/*void	Server::addClient(int client_socket, int server_socket){
	Client client(server_socket, client_socket);

	this->_clients[client_socket] = client;
	return;
}

//peut etre fait directement depuis le client client.set(buffer)
void	Server::setClient(int client_socket, char buffer){
	this->_clients[client_socket].set(buffer);
	return;
}

Client	Server::getClient(int socket_client) const{
	return this->_clients.at(socket_client);
}*/
