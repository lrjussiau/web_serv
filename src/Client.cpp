#include "../inc/Client.hpp"

// ------------------------------------------------------
// 					Canonical Form
// ------------------------------------------------------


Client::Client(void) : _socket(-1), _server_socket(-1){
	return;
}

Client::Client(int server_socket, int client_socket, std::string sessionId) : _socket(client_socket), _server_socket(server_socket),  _sessionId(sessionId), _sessionName("stranger"){
    std::cout << "client constructor called" << std::endl;
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
    this->_sessionName = src.getSessionName();
    this->_sessionId = src.getSessionId();
	return *this;
}

// ------------------------------------------------------
// 					     Setters
// ------------------------------------------------------

std::string cleanString(const std::string& input) {
    std::string output;
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        if (isprint(static_cast<unsigned char>(*it))) {
            output += *it;
        }
    }
    return output;
}

void    Client::setSessionName(std::string session_name){
    this->_sessionName = session_name;
    return;
}

void Client::setData(std::string filePath) {
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        std::cerr << "Failed to open request file: " << filePath << std::endl;
        return;
    }
    int i = 0;
	std::string line;

    while (std::getline(file, line)) {
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
        //cgi
        if (this->_requestedUrl == "/cgi" && line.find("input") != std::string::npos) {
            this->_buffer = line.substr(6, line.length() - 6);
            break;
        }
        //cookie
        if (this->_requestedUrl == "/cookie" && line.find("userName") != std::string::npos){
            this->_buffer = line.substr(9, line.length() - 9);
            break;
        }
        i++;
    }
    if (DEBUG) {
        std::cout << YEL << "Client Request:" << std::endl;
        std::cout << "Request Host: " << this->_requestHost << std::endl;
        std::cout << "Request Method: " << this->_requestMethod << std::endl;
        std::cout << "Requested URL: " << this->_requestedUrl << std::endl;
        std::cout << "Request Protocol: " << this->_requestProtocol << std::endl;
        std::cout << "Request Connection: " << this->_requestConnection << std::endl;
        std::cout << "Request Mimetype: " << this->_requestMimetype << RST << std::endl;
        std::cout << "Request Buffer: " << this->_buffer << RST << std::endl;
    }
    file.close();
}

void Client::parseCgiPostRequest(std::string &body){
    std::string line;
    std::istringstream buffer_stream(body);

    std::cout << "here is the body for cgi " << std::endl;
    while (std::getline(buffer_stream, line)){
        std::cout << line << std::endl;
    }
    return;
}


void Client::parsePostRequest(std::string path_to_request, std::string path) {
    std::string line;
    // bool inBody = false;
    bool inBound = false;
    bool findBoundary = false;


    std::ifstream request_file(path_to_request.c_str());
    if (!request_file.is_open()) {
        std::cerr << "Failed to open request file: " << path_to_request << std::endl;
        return;
    }

    while (std::getline(request_file, line)) {
        if (line.find(_boundary) != std::string::npos && findBoundary) {
            inBound = true;
        }
        if (line.find("Content-Type:") != std::string::npos && !findBoundary) {
            _boundary = line.substr(line.find("boundary=") + 9);
            findBoundary = true;
        }
        if (inBound) {
			parseBody(request_file, path);
			break;
        }
    }
    request_file.close();
}

void Client::parseBody(std::ifstream &request_file, std::string path) {
	std::string line;
	bool inBody = false;
	std::ofstream file;

	while (std::getline(request_file, line)) {
		if (line.find("Content-Disposition:") != std::string::npos) {
			getPathToUpload(line, path);
		if (_postName.empty())
			break;
			file.open(_postName.c_str(), std::ios::binary);
		}
		if (line == "\r") {
			inBody = true;
			continue;
		}
		if (inBody) {
			std::string boundary = _boundary;
			if (line.find(boundary.erase(_boundary.size() - 1)) != std::string::npos) {
				inBody = false;
				file.close();
				break;
			} else {
				file.write(line.c_str(), line.size());
				file.write("\n", 1);
			}
		}
	}
	if (file.is_open()) {
        file.close();
    }
}

void Client::getPathToUpload(std::string line, std::string path) {
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        if (token.find("filename=") != std::string::npos) {
            _postName = path + "/";
            _postName += token.substr(token.find("filename=") + 10);
            _postName.erase(_postName.size() - 1);
			if (DEBUG_REPONSE) {
				std::cout << "\t| " << "Path to upload : " << GRN << _postName << RST << std::endl;
			}
    	}
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

std::string	Client::getSessionId(void) const{
    return this->_sessionId;
}

std::string	Client::getSessionName(void) const{
    return this->_sessionName;
}