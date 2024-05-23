#include "../inc/Response.hpp"

void 		Response::buildResponse(void){
	std::string	final_reply;

	final_reply += this->_status_line.append("\r\n");
	for(std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it){
		if (!it->second.empty()){
			final_reply += it->first;
			final_reply += it->second;
			final_reply += "\r\n";
		}
	}
	final_reply.append("\r\n");
	final_reply += this->_content;
	this->_final_reply = final_reply;
}

Response::Response(void) {
	return;
}

Response::Response(Client client, ServerConfig server) : _client(client) {
	std::string	path;
	bool 		find_location = false;
	Location 	location;

	std::cout << ORG << "[Server : " << server.server_name << "] build a response" << RST << std::endl;
	if (client.getRequestMethod() != "GET" && client.getRequestMethod() != "POST" && client.getRequestMethod() != "DELETE") {
		createContent(server.root + server.error_pages[405], 405, "Method Not Allowed");
	}
	// 	if () {																			// Look For Content-Length
	//  createContent(server.root + server.error_pages[413], 413, "Request Entity Too Large");

	// Implement redirect 

	for (std::map<std::string, Location>::iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
		if (client.getRequestedUrl().find(it->first) != std::string::npos) {
 			location = it->second;
			find_location = true;
			break;
		}
	}
	if (!find_location) {
		path = server.root + client.getRequestedUrl();
	} else {
		path = location.root + client.getRequestedUrl();
	}
	PathType pathType = getPathType(path);
	switch (pathType) {
		case PATH_IS_FILE:
			break;
		case PATH_IS_DIRECTORY:
			if (getPathType(path + "/index.html") == PATH_IS_FILE) {
                path += "/index.html";
			} else {
				if (location.autoindex) {
					createContent(path, 0, "autoindex");
				} else {
					createContent(server.root + server.error_pages[403], 403, "Forbidden");
				}
			}
			break;
		case PATH_NOT_FOUND:
			createContent(server.root + server.error_pages[404], 404, "Not Found");
			break;
	}
	for (std::vector<std::string>::iterator it = location.cgi_extensions.begin(); it != location.cgi_extensions.end(); ++it) {
		if (path.find(*it) != std::string::npos) {
			createContent(path, 0, "CGI");
		}
	}
	if (checkMimeType(client.getRequestMimetype()) == false) {
		createContent(server.root + server.error_pages[415], 415, "Unsupported Media Type");
	}
	if (client.getRequestMethod() == "POST") {
		// createContent(201)
	} else {
		createContent(path, 200, "OK");
	}
}

Response::~Response(void) {
	return;
}

Response::Response(const Response& src) {
	if (this != &src)
		*this = src;
	return;
}

void Response::createContent(std::string path, int status_code, std::string status_message) {
	std::ifstream	file;
	std::string		line;
	std::string		content;

	file.open(path.c_str());
	if (!file.is_open()) {
		std::cerr << RED << "Error: Could open file with this file : " << path << RST << std::endl;
		return;
	}
	while (std::getline(file, line)) {
		content += line;
		content += "\n";
	}
	this->_content = content;

	if (status_message == "autoindex") {
		// create autoindex
	} else if (status_message == "CGI") {
		// create CGI
	} else {
		this->_status_line = "HTTP/1.1 " + std::to_string(status_code) + " " + status_message;
		init_headers();
	}
}


void Response::init_headers(void) {
	this->_headers["Date: "] = getTime();
	this->_headers["Content-Length: "] = std::to_string(this->_content.length());
	this->_headers["Content-Type: "] = this->_client.getRequestMimetype();
	this->_headers["Connection: "] = this->_client.getRequestConnection();
	if (DEBUG) {
		std::cout << CYA << "Server Response: "<< std::endl;
		std::cout << this->_status_line << std::endl;
		for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
			std::cout << it->first << it->second << std::endl;
		}
		// std::cout << CYA << this->_content << RST << std::endl;
	}
	buildResponse();
}

std::string	Response::getFinalReply(void) const{
	return this->_final_reply;
}

bool Response::checkMimeType(std::string mime) {
	if (mime == "text/html" || mime == "text/css" 
		|| mime == "text/javascript" || mime == "image/png" 
		|| mime == "image/jpg" || mime == "image/jpeg" || mime == "image/gif" 
		|| mime == "image/bmp" || mime == "image/webp" || mime == "image/avif"
		|| mime == "image/svg+xml" || mime == "image/x-icon" || mime == "application/xml" 
		|| mime == "application/json" || mime == "application/javascript" 
		|| mime == "application/x-javascript" || mime == "application/x-www-form-urlencoded"
		|| mime == "multipart/form-data" || mime == "text/plain") {
		return true;
	}
	if (DEBUG) {
		std::cout << RED << "Unsupported Media Type: " << mime << RST << std::endl;
	}
	return false;
}
