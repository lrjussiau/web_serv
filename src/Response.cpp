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

// 200 - 201 - 400 - 404 - 405 - 408 - 413 - 500 - 505

Response::Response(Client client, ServerConfig server) {
	std::cout << server.server_name << std::endl;
	std::string path_error_page = "src/html/error_page/";
	if (client.getRequestProtocol() != "HTTP/1.1") {
		buildStatusLine(505, "HTTP Version Not Supported");
		createContent( path_error_page + "505.html");
	// } else if (client.getRequestHost() != server.server_name) {								// Add Server Name				
	// 	buildStatusLine(500, "Internal Server");
	//  createContent( path_error_page + "500.html");
	// } else if () {																			// Look For Time
	// 	buildStatusLine(408, "Request Timeout");
	//  createContent( path_error_page + "408.html");
	} else if (client.getRequestMethod() != "GET" && client.getRequestMethod() != "POST") {
		buildStatusLine(405, "Method Not Allowed");
		createContent(path_error_page + "405.html");
	} else if (!is_file_exist(client.getRequestedUrl())) {
		buildStatusLine(404, "Not Found");
		createContent( path_error_page + "404.html");
	// } else if () {																			// Look For Content-Length
	// 	buildStatusLine(413, "Request Entity Too Large");
	//  createContent( path_error_page + "413.html");
	} else if (client.getRequestMethod() == "POST") {
		buildStatusLine(201, "OK");
			//createContent();
	} else if (client.getRequestMethod() == "GET") {
		buildStatusLine(200, "OK");
		if (client.getRequestedUrl() == "/")
			createContent("src/html/index.html");
		else if (client.getRequestedUrl() != "/favicon.ico")
			createContent("src/html" + client.getRequestedUrl());
	} else {
		buildStatusLine(400, "BAD REQUEST");
		createContent( path_error_page + "400.html");
	}
	return;
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
		throw Except("Fail to open file");
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

bool	Response::is_file_exist(const std::string& path) {
	std::ifstream file("src/html/" + path);
	if (file.is_open()) {
		file.close();
		return true;
	} else {
		return false;
	}
}