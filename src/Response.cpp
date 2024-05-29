#include "../inc/Response.hpp"

// ------------------------------------------------------
// 					Canonical Form
// ------------------------------------------------------

Response::Response(void) {
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

// ------------------------------------------------------
// 					Constructor
// ------------------------------------------------------

Response::Response(Client *client, ServerConfig server) : _client(client) , _server(server) {
	std::string	path;
	Location* 	location;

	std::cout << ORG << "[Server : " << server.server_name << "] build a response" << RST << std::endl;
	std::cout << ORG << "Client :" << _client->getSessionName() << RST << std::endl;
	if (DEBUG_REPONSE) {
		std::cout << GRN << "Debug Reponse Path :" << RST << std::endl;
	}
	if (isMethodWrong())
		return;
	if (DEBUG_REPONSE) {
		std::cout << "\t| Check Method : " << GRN << "OK" << RST << std::endl;
	}
	if (isCookie(client)) {
		std::cout << ORG << "Client :" << _client->getSessionName() << RST << std::endl;
		return;
	}
	if (DEBUG_REPONSE) {
		std::cout << "\t| Check Cookie : " << GRN << "OK" << RST << std::endl;
	}
	// 	if () {																			// Look For Content-Length
	//  createContent(server.root + server.error_pages[413], 413, "Request Entity Too Large");

	// Implement redirect
	if (isCGI())
		return;
	if (DEBUG_REPONSE) {
		std::cout << "\t| Check cgi : " << GRN << "OK" << RST << std::endl;
	}
	location = findLocation();
	if (DEBUG_REPONSE) {
		if (location != NULL)
			std::cout << "\t| Location : " << GRN << location->path << RST << std::endl;
		else
			std::cout << "\t| Location : " << GRN << "No location found" << RST << std::endl;
	}
	path = findPath(location);
	if (DEBUG_REPONSE) {
		std::cout << "\t| Path : " << GRN << path << RST << std::endl;
	}


	PathType pathType = getPathType(path);
	if (client->getRequestMethod() == "GET") {
		switch (pathType) {
			case PATH_IS_FILE:
				if (DEBUG_REPONSE) {
					std::cout << "\t| " << GRN << "\t is a file" << RST << std::endl;
				}
				break;
			case PATH_IS_DIRECTORY:
				if (getPathType(path + "/index.html") == PATH_IS_FILE) {
					if (DEBUG_REPONSE) {
						std::cout << "\t| " << GRN << "\t is a directory, but find index.html inside" << RST << std::endl;
					}
					path += "/index.html";
				} else {
					handleDirectory(path, location);
					return;
				}
				break;
			case PATH_NOT_FOUND:
				if (DEBUG_REPONSE) {
					std::cout << "\t| " << GRN << "\t not existing file or directory, 404" << RST << std::endl;
				}
				createContent(server.root + server.error_pages[404], 404, "Not Found");
				return;
		}
	}
	if (checkMimeType())
		return;
	if (DEBUG_REPONSE) {
		std::cout << "\t| Check Mime Type : " << GRN << "OK" << RST << std::endl;
	}
	if (client->getRequestMethod() == "POST") {
		if (DEBUG_REPONSE) {
			std::cout << "\t| " << "Path to upload : " << GRN << path << RST << std::endl;
			std::cout << "\t| " << GRN << "Create a 201 request" << RST << std::endl;
		}
		client->parsePostRequest(PATH_TO_REQUESTS, path);
		createContent("", 201, "Created");
		return;
	} else {
		if (DEBUG_REPONSE) {
		std::cout << "\t| " << GRN << "Create a 200 request" << RST << std::endl;
		}
		createContent(path, 200, "OK");
	}
}

// ------------------------------------------------------
// 						 Checker
// ------------------------------------------------------

bool Response::checkMimeType() {

	std::string mime = _client->getRequestMimetype();

	if (mime != "text/html" && mime != "text/css" 
		&& mime != "text/javascript" && mime != "image/png" 
		&& mime != "image/jpg" && mime != "image/jpeg" && mime != "image/gif" 
		&& mime != "image/bmp" && mime != "image/webp" && mime != "image/avif"
		&& mime != "image/svg+xml" && mime != "image/x-icon" && mime != "application/xml" 
		&& mime != "application/json" && mime != "application/javascript" 
		&& mime != "application/x-javascript" && mime != "application/x-www-form-urlencoded"
		&& mime != "multipart/form-data" && mime != "text/plain") {
		createContent(_server.root + _server.error_pages[415], 415, "Unsupported Media Type");
		return true;
	}
	return false;
}

bool	Response::isMethodWrong() {
	if (_client->getRequestMethod() != "GET" && _client->getRequestMethod() != "POST" && _client->getRequestMethod() != "DELETE") {
		createContent(_server.root + _server.error_pages[405], 405, "Method Not Allowed");
		return true;
	}
	return false;
}

//close?file
std::string readFile(const std::string &path) {
    std::ifstream file(path);
	file.open(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string replacePlaceholder(const std::string &html, const std::string &placeholder, const std::string &value) {
    std::string result = html;
    size_t pos = result.find(placeholder);
    while (pos != std::string::npos) {
        result.replace(pos, placeholder.length(), value);
        pos = result.find(placeholder, pos + value.length());
    }
    return result;
}

bool	Response::isCookie(Client *client) {
	std::cout << "mur " << client->getRequestedUrl() << std::endl;
	if (client->getRequestedUrl() == "/cookie.html" || client->getRequestedUrl() == "/cookie" ){
		try {
			std::string html = readFile("./website/html/cookie.html");
			if (client->getRequestMethod() == "GET"){
				std::cout << "I am in the get for cookie" <<std::endl;
				std::string name  = client->getSessionName();
				std::cout << "client sesh name : " << client->getSessionName() << std::endl;
				this->_content = replacePlaceholder(html, "{{message}}", name);
				createContent("", 200, "COOKIE");
			}
			else{
				std::cout << "I am in the post for cookie" <<std::endl;
				client->setSessionName(_client->getBuffer());
				std::cout << "client session: " << client->getSessionName() << std::endl;
				this->_content = replacePlaceholder(html, "{{message}}",  client->getSessionName());
				createContent("", 201, "COOKIE");
			}
			} catch (const std::exception &e) {
				std::cout << "Error 500" << e.what() <<std::endl;
		}
		return true;
	}
	return false;
}

bool	Response::isCGI() {
	if (_client->getRequestMethod() == "POST" && _client->getRequestedUrl() == "/cgi") {
		this->_content = generateCgi(_client->getBuffer());
		createContent("", 201, "CGI");
		return true;
	}
	return false;
}

void	Response::handleDirectory(std::string path, Location *location) {
	if (location->autoindex) {
		if (DEBUG_REPONSE) {
			std::cout << "\t| " << GRN << "\t Is a directory: Auto index activate" << RST << std::endl;
		}
		char cwd[1024];
		std::string auto_index;
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			auto_index = cwd;
		}
		if (path[0] == '.')
			path.erase(0, 1);
		auto_index += path;
		if (DEBUG_REPONSE) {
			std::cout << "\t| "<< "\t Path to auto index : " << GRN << auto_index << RST << std::endl;
		}
		generateAutoIndex(auto_index);
	} else {
		if (DEBUG_REPONSE) {
			std::cout << "\t| " << GRN << "\t is a directory: Auto index desactivate, 403" << RST << std::endl;
		}
		createContent(_server.root + _server.error_pages[403], 403, "Forbidden");
	}
	return;
}

// ------------------------------------------------------
// 					  Create Reponse
// ------------------------------------------------------

void Response::createContent(std::string path, int status_code, std::string status_message) {
    std::ifstream file;
    std::ostringstream content_stream;
    std::string content;

    if (status_message != "autoindex" && status_message != "CGI" && status_message != "COOKIE" && _client->getRequestMethod() != "POST" ) {
        file.open(path.c_str(), std::ios::binary);
        if (!file.is_open()) {
            std::cerr << RED << "Error: Could not open file: " << RST << path << std::endl;
            return;
        }
        content_stream << file.rdbuf();
        content = content_stream.str();
        this->_content = content;
		file.close();
		if (DEBUG_REPONSE) {
			std::cout << "\t| " << "Create content : " << GRN << "OK" << RST << std::endl;
		}
    }
	createStatusLine(status_code, status_message);
    init_headers();
}

void Response::createStatusLine(int status_code, std::string status_message) {
	if (DEBUG_REPONSE) {
		std::cout << "\t| " << "Create status line : " << GRN << "OK" << RST << std::endl;
	}
	std::stringstream ss;
	ss << "HTTP/1.1 " << status_code << " " << status_message;
	this->_status_line = ss.str();
}

void Response::init_headers(void) {
	this->_headers["Date: "] = getTime();
	this->_headers["Content-Length: "] = std::to_string(this->_content.length());
	this->_headers["Content-Type: "] = _client->getRequestMimetype();
	this->_headers["Connection: "] = _client->getRequestConnection();
	this->_headers["Set-cookie: "] = "sessionId=" + _client->getSessionId() + "; HttpOnly";
	if (DEBUG_REPONSE) {
		std::cout << "\t| " << "Create header : " << GRN << "OK" << RST << std::endl;
	}
	buildResponse();
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
	if (DEBUG_REPONSE) {
		std::cout << "\t| " << GRN << "Build request : " << "OK" << RST << std::endl;
		std::cout << "\t|--------------------------" << std::endl;
	}
	if (DEBUG) {
		std::cout << CYA << "Server Response: "<< std::endl;
		std::cout << this->_status_line;
		for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it) {
			std::cout << it->first << it->second << std::endl;
		}
		// std::cout << CYA << this->_content << RST << std::endl;
	}
}

// ------------------------------------------------------
// 					    Getter
// ------------------------------------------------------

std::string	Response::getFinalReply(void) const{
	return this->_final_reply;
}

Location* Response::findLocation() { 
	for (std::map<std::string, Location>::iterator it = _server.locations.begin(); it != _server.locations.end(); ++it) {
		size_t pos = _client->getRequestedUrl().find(it->first);

		if (pos != std::string::npos) {
			if (!it->second.redirect.empty()){
				std::cout << GRN << "A redirect response is made" << RST << std::endl;
				buildRedirectResponse(it->second.redirect);
				return (NULL);
			}
			if (_client->getRequestedUrl()[it->first.size()] != '.')
				return &(it->second);
		}
	}
	return (NULL);
}

std::string Response::findPath(Location *location) {
	if (location == NULL) {
		return (_server.root.erase(_server.root.size() - 1) + _client->getRequestedUrl());
	} else {
		if (location->path == "/cgi") {
			return (location->cgi_pass.erase(location->cgi_pass.size() - 1) + _client->getRequestedUrl());
		}
		return (location->root.erase(location->root.size() - 1) + _client->getRequestedUrl());
	}
}

// ------------------------------------------------------
// 					    Utils
// ------------------------------------------------------

void	Response::buildRedirectResponse(std::string redirect_path){
	this->_status_line = "HTTP/1.1 301 OK";
	this->_headers["Location: "] = redirect_path;
	init_headers();
	return;
}

std::string Response::generateCgi(std::string input_string){
    std::string cgi_page;
    std::string encoded_string;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // Child process
        close(pipefd[0]); // Close unused read end

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]); // Close the write end of the pipe

        // Prepare the arguments for execve
        char *args[] = {const_cast<char *>("python3"),
                        const_cast<char *>("website/cgi/base64encoder.py"),
                        const_cast<char *>(input_string.c_str()),
                        NULL};

        // Execute the Python script
        execve("/usr/local/munki/Python.framework/Versions/3.8/bin/python3.8", args, NULL);
        // If execve returns, it must have failed.
        perror("execve");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pipefd[1]); // Close unused write end
        char buffer[128];
        ssize_t count;
        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            encoded_string += buffer;
        }
        close(pipefd[0]); // Close read end

        // Wait for child process to finish
        int status;
        waitpid(pid, &status, 0);
    }
	/*if (!encoded_string.empty() && encoded_string.back() == '\n') {
            encoded_string.erase(encoded_string.size() - 1);
    }*/
	std::cout << "encoded string " << encoded_string << std::endl;
    //cgi_page += "<!DOCTYPE html>\n<html>\n<h1>Here is your encoded string</h1>\n<body>\n";
    //cgi_page += "<h3>" + encoded_string + "</h3>\n";
    //cgi_page += "</body>\n</html>\n";

    return encoded_string;
}

// ------------------------------------------------------
// 					    Auto Index
// ------------------------------------------------------

std::vector<std::string>	getFiles(std::string dir_requested){
	 std::vector<std::string>	files;
	 struct dirent* 			entry;

    DIR* dir = opendir(dir_requested.c_str());
    if (dir == NULL) {
		perror("opendir");
        std::cerr << RED << "Error: Unable to open directory " << dir_requested << RST <<std::endl;
        return files;
    }
	if (DEBUG_REPONSE) {
		std::cout << "\t| " << "\t Find directory : " << GRN << "OK" << RST << std::endl;
	}
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            files.push_back(name);
        }
    }
    closedir(dir);
	if (DEBUG_REPONSE) {
		std::cout << "\t| " << "\t Collect files information : " << GRN << "OK" << RST << std::endl;
	}
    return files;
}

void	Response::generateAutoIndex(std::string dir_requested){
	std::string					auto_index;
    std::vector<std::string>	files = getFiles(dir_requested);

	auto_index += "<!DOCTYPE html>\n<html>\n<head><title>" + dir_requested + "/</title></head>\n<body>\n";
	auto_index += "<h1>"+ dir_requested + "/</h1><hr><pre><a href=\"../\">../</a>";
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it){
		auto_index += "<a href=\"" + *it + "/\">" + *it + "/</a>\n";     
	}
	auto_index += "</body>\n</html>\n";
	this->_content = auto_index;
	createContent("", 200, "autoindex");
}
