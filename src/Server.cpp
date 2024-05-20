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
	this->_clients = src.getClients();
	return *this;
}

std::map<int, Client> Server::getClients(void) const{
	return this->_clients;
}

std::vector<int>	Server::getSockets(void) const{
	return this->_sockets;
}

/*void	Server::createServer(ServerConfig server_config){
	for (size_t i = 0; i < server_config.ports.size(); ++i) {
		launchSocket(server_config.ports[i], server_config.ip, server_config.IPv4);
		catch (const std::exception& e){
        	std::cerr << e.what() << std::endl;
		}
    }
	return;
}*/

int Server::launchSocket(uint16_t port, const char *ip, bool IPv4) {
	struct sockaddr_in sa;
	int					socket;

    // Préparaton de l'adresse et du port pour la socket de notre serveur
    memset(&sa, 0, sizeof sa);
	if (IPv4 == true){
		inet_pton(AF_INET, ip, &sa.sin_addr);
    	sa.sin_family = AF_INET;
	}
	else{
		inet_pton(AF_INET6, ip, &sa.sin_addr);
		sa.sin_family = AF_INET6;
	}
    sa.sin_port = htons(port);
    // Création de la socket
	socket = socket(sa.sin_family, SOCK_STREAM, 0);
    this->_sockets.insert(socket);
    if (socket == -1) {
        throw ServerSocketError();
    }
    if (bind(socket, (struct sockaddr *)&sa, sizeof sa)) {
    	throw ServerBindingError();
	}
    if (listen(socket, 10)) {
        throw ServerListeningError();
    }
	printf("[Server] Created server socket fd: %d\n", socket);
	return (0);
}

void	Server::addClient(int client_socket, int server_socket){
	Client client(server_socket, client_socket);

	this->_clients[client_socket] = client;
	return;
}

//peut etre fait directement depuis le client client.set(buffer)
void	Server::setClient(int client_socket, char buffer){
	this->_clients[client_socket].set(buffer);
	return;
}

Client	getClient(int socket_client) const{
	return this->_clients[socket_client];
}
