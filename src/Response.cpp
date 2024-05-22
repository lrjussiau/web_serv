#include "../inc/Response.hpp"

enum PathType {
    PATH_NOT_FOUND,
    PATH_IS_FILE,
    PATH_IS_DIRECTORY
};

PathType getPathType(const std::string& path) {
	struct stat pathStat;
	if (stat(path.c_str(), &pathStat) != 0) {
		return PATH_NOT_FOUND;
	}
	if (S_ISREG(pathStat.st_mode)) {
		return PATH_IS_FILE;
	}
	if (S_ISDIR(pathStat.st_mode)) {
		return PATH_IS_DIRECTORY;
	}
	return PATH_NOT_FOUND;
}

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

Response::Response(Client client, ServerConfig server) {
	std::string	path;
	bool 		find_location = false;
	Location 	location;

	std::cout << GRN << "I am building a response from: " << server.server_name << std::endl;
	if (client.getRequestMethod() != "GET" && client.getRequestMethod() != "POST" && client.getRequestMethod() != "DELETE") {
		buildStatusLine(405, "Method Not Allowed");
		createContent(server.root + server.error_pages[405]);
	}
	// 	if () {																			// Look For Content-Length
	// 	buildStatusLine(413, "Request Entity Too Large");
	//  createContent( path_error_page + "413.html");

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
					// createContent(autoindex)
				} else {
					buildStatusLine(403, "Forbidden");
					createContent(server.root + server.error_pages[403]);
				}
			}
			break;
		case PATH_NOT_FOUND:
			buildStatusLine(404, "Not Found");
			createContent(server.root + server.error_pages[404]);
			break;
	}
	// If extention CGI
		// createContent(CGI) 
	// If wrong mime type
		// createContent(415)
	if (client.getRequestMethod() == "POST") {
		// createContent(201)
	} else {
		buildStatusLine(200, "OK");
		createContent(path);
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

void Response::buildStatusLine(int status_code, std::string status_message) {
	this->_status_line = "HTTP/1.1 " + std::to_string(status_code) + " " + status_message;
}

void Response::createContent(std::string path) {
	std::ifstream file(path);
	std::string line;

	if (file.is_open()) {
		while (std::getline(file, line)) {
			this->_content += line + "\n";
		}
		file.close();
	} else {
		// throw Except("Fail to open file");
	}
	init_headers();
}


void Response::init_headers(void) {
	this->_headers["Date: "] = getTime();
	this->_headers["Content-Length: "] = std::to_string(this->_content.length());
	this->_headers["Content-Type: "] = "text/html";
	this->_headers["Connection: "] = "keep-alive";
	if (DEBUG) {
		std::cout << CYA << "Server Response: "<< std::endl;
		std::cout << this->_status_line << std::endl;
		for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
			std::cout << it->first << it->second << std::endl;
		}
		std::cout << this->_content << RST << std::endl;
	}
	buildResponse();
}

std::string	Response::getFinalReply(void) const{
	return this->_final_reply;
}
