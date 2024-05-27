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
	this->_requestHost = src.getRequestHost();
	this->_requestConnection = src.getRequestConnection();
	this->_requestMimetype = src.getRequestMimetype();
	this->_postName = src.getPostName();
	this->_buffer = src.getBuffer();
	return *this;
}

// ------------------------------------------------------
// 					     Setters
// ------------------------------------------------------


// void Client::setData(char *buffer){
// 	std::string buffer_str(buffer);
// 	std::string line;
// 	std::istringstream buffer_stream(buffer_str);
// 	std::string body;
// 	std::string boundaryValue;

// 	bool inBody = false;
// 	int i = 0;
// 	while(std::getline(buffer_stream, line)){
// 		if (i == 0) {
// 			std::istringstream line_stream(line);
// 			line_stream >> this->_requestMethod >> this->_requestedUrl >> this->_requestProtocol;
// 		}
// 		if (line.find("Host:") != std::string::npos){
// 			this->_requestHost = line.substr(6);
// 		}
// 		if (line.find("Connection:") != std::string::npos){
// 			this->_requestConnection = line.substr(12);
// 		}
//     	if (line.find("Accept:") != std::string::npos) {
// 			std::string acceptValue = line.substr(line.find("Accept:") + 7);
// 			std::istringstream iss(acceptValue);
// 			std::string token;
// 			if (std::getline(iss, token, ',')) {
// 				token.erase(0, token.find_first_not_of(' '));
// 				this->_requestMimetype = token;
// 			}
// 		}
// 		if (this->_requestMethod == "POST" && !boundaryValue.empty() && line.find(boundaryValue) != std::string::npos) {
// 			inBody = true;
// 		}
// 		if (line.find("Content-Type:") != std::string::npos) {
// 			boundaryValue = line.substr(line.find("boundary=") + 9);
// 		}
// 		if (inBody) {
// 			body += line + "\n";
// 		}
// 		i++;
// 	}
// 	if (this->_requestMethod == "POST")
// 		parsePostRequest(body);
// 	if (DEBUG) {
// 		std::cout << YEL << "Client Request:" << std::endl;
// 		std::cout << "Request Host: " << this->_requestHost << std::endl;
// 		std::cout << "Request Method: " << this->_requestMethod << std::endl;
// 		std::cout << "Requested URL: " << this->_requestedUrl << std::endl;
// 		std::cout << "Request Protocol: " << this->_requestProtocol << std::endl;
// 		std::cout << "Request Connection: " << this->_requestConnection << std::endl;
// 		std::cout << "Request Mimetype: " << this->_requestMimetype << RST << std::endl;
// 	}
// }

// void Client::parsePostRequest(std::string &body){
// 	std::string line;
// 	std::istringstream buffer_stream(body);

// 	bool inBody = false;
// 	while(std::getline(buffer_stream, line)){

// 		if (line.find("Content-Disposition:") != std::string::npos) {
// 			std::istringstream iss(line);
// 			std::string token;
// 			while (iss >> token) {
// 				if (token.find("filename=") != std::string::npos) {
// 					_postName = token.substr(token.find("filename=") + 10);
// 					_postName.erase(_postName.size() - 1);
// 				}
// 				if (!_postName.empty())
// 					break;
// 			}
// 		}
//         if (inBody) {
//             _buffer += line + "\n";
//         }
// 		if (line == "\r") {
// 			inBody = true;
// 		}
// 	}
// }

void Client::setData(char *buffer) {
    std::string buffer_str(buffer, 8000);
    std::string line;
    std::istringstream buffer_stream(buffer_str);
    std::string body;
    std::string boundaryValue;

    bool inBody = false;
    int i = 0;
    while (std::getline(buffer_stream, line)) {
        if (i == 0) {
            std::istringstream line_stream(line);
            line_stream >> this->_requestMethod >> this->_requestedUrl >> this->_requestProtocol;
        }
        if (line.find("Host:") != std::string::npos) {
            this->_requestHost = line.substr(6);
        }
        if (line.find("Connection:") != std::string::npos) {
            this->_requestConnection = line.substr(12);
        }
        if (line.find("Accept:") != std::string::npos) {
            std::string acceptValue = line.substr(line.find("Accept:") + 7);
            std::istringstream iss(acceptValue);
            std::string token;
            if (std::getline(iss, token, ',')) {
                token.erase(0, token.find_first_not_of(' '));
                this->_requestMimetype = token;
            }
        }
        if (this->_requestMethod == "POST" && !boundaryValue.empty() && line.find(boundaryValue) != std::string::npos) {
            inBody = true;
        }
        if (line.find("Content-Type:") != std::string::npos) {
            boundaryValue = line.substr(line.find("boundary=") + 9);
        }
        if (inBody) {
            body += line + "\n";
        }
        i++;
    }
    if (this->_requestMethod == "POST")
		_boundary = boundaryValue;	
        parsePostRequest(body);
    if (DEBUG) {
        std::cout << YEL << "Client Request:" << std::endl;
        std::cout << "Request Host: " << this->_requestHost << std::endl;
        std::cout << "Request Method: " << this->_requestMethod << std::endl;
        std::cout << "Requested URL: " << this->_requestedUrl << std::endl;
        std::cout << "Request Protocol: " << this->_requestProtocol << std::endl;
        std::cout << "Request Connection: " << this->_requestConnection << std::endl;
        std::cout << "Request Mimetype: " << this->_requestMimetype << RST << std::endl;
    }
}

void Client::parsePostRequest(std::string &body) {
    std::string line;
    std::istringstream buffer_stream(body);
    bool inBody = false;
    std::string boundaryValue = "--" + _boundary;
    std::ofstream file;
    
    while (std::getline(buffer_stream, line)) {
        if (line.find("Content-Disposition:") != std::string::npos) {
            std::istringstream iss(line);
            std::string token;
            while (iss >> token) {
                if (token.find("filename=") != std::string::npos) {
                    _postName = token.substr(token.find("filename=") + 10);
                    _postName.erase(_postName.size() - 1);
                    file.open(_postName, std::ios::binary); // Open file in binary mode
                }
                if (!_postName.empty())
                    break;
            }
        }
        if (line == "\r") {
            inBody = true; // File content starts after an empty line
            continue;
        }
        if (inBody) {
            if (line.find(boundaryValue) != std::string::npos) {
                inBody = false;
                file.close();
            } else {
                file.write(line.c_str(), line.size()); // Write binary data to file
                file.write("\n", 1); // Maintain the new line character
            }
        }
    }
    if (file.is_open()) {
        file.close(); // Ensure the file is closed if the boundary was not found
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

std::string Client::getRequestConnection(void) const{
	return this->_requestConnection;
}

std::string Client::getRequestMimetype(void) const{
	return this->_requestMimetype;
}

std::string Client::getPostName(void) const{
	return this->_postName;
}

std::string Client::getBuffer(void) const{
	return this->_buffer;
}