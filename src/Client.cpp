#include "../inc/Client.hpp"

// ------------------------------------------------------
// 					Canonical Form
// ------------------------------------------------------


Client::Client(void) : _socket(-1), _server_socket(-1){
	return;
}

Client::Client(int server_socket, int client_socket) : _socket(client_socket), _server_socket(server_socket){
	return;
}

Client::~Client(void){
	return;
}

Client::Client(const Client& src){
	if (this != &src)
		*this = src;
	return;
}

Client&	Client::operator=(const Client& src){
	this->_socket = src.getSocket();
	this->_server_socket = src.getServerSocket();
	this->_requestedUrl = src.getRequestedUrl();
	this->_requestMethod = src.getRequestMethod();
	this->_requestProtocol = src.getRequestProtocol();
	return *this;
}

// ------------------------------------------------------
// 					     Setters
// ------------------------------------------------------


void Client::setData(char *buffer){
	std::string buffer_str(buffer);
	std::string line;
	std::istringstream buffer_stream(buffer_str);
	while(std::getline(buffer_stream, line)){
		if (line.find("Host:") != std::string::npos){
			this->_requestHost = line.substr(6);
		}
		else {
			std::istringstream line_stream(line);
			line_stream >> this->_requestMethod >> this->_requestedUrl >> this->_requestProtocol;
		}
	}
	if (DEBUG) {
		std::cout << YEL << "Client Request:" << std::endl;
		std::cout << "Request Host: " << this->_requestHost << std::endl;
		std::cout << "Request Method: " << this->_requestMethod << std::endl;
		std::cout << "Requested URL: " << this->_requestedUrl << std::endl;
		std::cout << "Request Protocol: " << this->_requestProtocol << RST << std::endl;
	}
}

// ------------------------------------------------------
// 					     Getters
// ------------------------------------------------------

std::string Client::getRequestedUrl(void) const{
	return this->_requestedUrl;
}

std::string Client::getRequestMethod(void) const{
	return this->_requestMethod;
}

std::string Client::getRequestProtocol(void) const{
	return this->_requestProtocol;
}

int	Client::getSocket(void) const{
	return this->_socket;
}

int	Client::getServerSocket(void) const{
	return this->_server_socket;
}

std::string Client::getRequestHost(void) const{
	return this->_requestHost;
}