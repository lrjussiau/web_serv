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

void	Response::buildRedirectResponse(std::string redirect_path){
	this->_status_line = "HTTP/1.1 301 OK";
	this->_headers["Location: "] = redirect_path;
	init_headers();
	return;
}

std::string generateCgi(std::string input_string){
    //std::string cgi_page;
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


std::vector<std::string>	getFiles(std::string dir_requested){
	 std::vector<std::string>	files;
	 struct dirent* 			entry;

    DIR* dir = opendir(dir_requested.c_str());
    if (dir == NULL) {
		perror("opendir");
        std::cerr << RED << "Error: Unable to open directory " << dir_requested << RST <<std::endl;
        return files;
    }
	std::cout << "Directory opened" << std::endl;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            files.push_back(name);
        }
    }
    closedir(dir);
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
	createContent(NULL, 200, "autoindex");
}

Response::~Response(void) {
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
	
	if (client.getRequestMethod() == "POST" && client.getRequestedUrl() == "/cgi/base64encoder.py") {
		this->_content = generateCgi(client.getBuffer());
		createContent("", 201, "CGI");
		return;
	}
	for (std::map<std::string, Location>::iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
		if (client.getRequestedUrl() == it->first) {
			if (!it->second.redirect.empty()){
				std::cout << GRN << "A redirect response is made" << RST << std::endl;
				buildRedirectResponse(it->second.redirect);
				return;
			}

 			location = it->second;
			find_location = true;
			std::cout << "Location found : " << location.root << std::endl;
			break;
		}
	}
	std::cout << "req url: " << client.getRequestedUrl() << std::endl;
	if (!find_location) {
		path = server.root.erase(server.root.size() - 1) + client.getRequestedUrl();
	} else {
		path = location.root.erase(location.root.size() - 1) + client.getRequestedUrl();
	}
	PathType pathType = getPathType(path);
	std::cout << "path type: " << path << std::endl;
	switch (pathType) {
		case PATH_IS_FILE:
			break;
		case PATH_IS_DIRECTORY:
			if (client.getRequestMethod() == "POST") {
				break;
			} else if (getPathType(path + "/index.html") == PATH_IS_FILE) {
                path += "/index.html";
			} else {
				if (location.autoindex) {
					char cwd[1024];
					std::string auto_index;
					if (getcwd(cwd, sizeof(cwd)) != NULL) {
						auto_index = cwd;
					}
					auto_index += location.root + client.getRequestedUrl();
					// std::cout << "AutoIndex: " << auto_index << std::endl;
					generateAutoIndex(auto_index);
				} else {
					createContent(server.root + server.error_pages[403], 403, "Forbidden");
				}
				return;
			}
			break;
		case PATH_NOT_FOUND:
			std::cout << " yeah boy" << std::endl;
			createContent(server.root + server.error_pages[404], 404, "Not Found");
			break;
	}
	if (checkMimeType(client.getRequestMimetype()) == false) {
		createContent(server.root + server.error_pages[415], 415, "Unsupported Media Type");
	}
	if (client.getRequestMethod() == "POST") {
		createContent("", 201, "Created");
		return;
	} else {
		createContent(path, 200, "OK");
	}
}



Response::Response(const Response& src) {
	if (this != &src)
		*this = src;
	return;
}

void Response::createContent(std::string path, int status_code, std::string status_message) {
	std::cout << "Inside createContent" << std::endl;
    std::ifstream file;
    std::ostringstream content_stream;
    std::string content;

    if (status_message == "autoindex") {
        std::stringstream ss;
        ss << "HTTP/1.1 " << status_code << " OK";
        this->_status_line = ss.str();
    } else if (status_message == "CGI") {
		std::cout << "helloh" << std::endl;
		std::stringstream ss;
        ss << "HTTP/1.1 " << status_code << " OK";
        this->_status_line = ss.str();
        std::cout << "content?: " << content << std::endl;
	} else if (_client.getRequestMethod() == "POST") {
		std::cout << BLU << " yeah post" << RST << std::endl;
		std::stringstream ss;
        ss << "HTTP/1.1 " << status_code << " " << status_message;
        this->_status_line = ss.str();
    } else {
        file.open(path.c_str(), std::ios::binary);
        if (!file.is_open()) {
            std::cerr << RED << "Error: Could not open file: " << RST << path << std::endl;
            return;
        }

        content_stream << file.rdbuf();
        content = content_stream.str();

        this->_content = content;

        std::stringstream ss;
        ss << "HTTP/1.1 " << status_code << " " << status_message;
        this->_status_line = ss.str();
    }

    init_headers();
}

void Response::init_headers(void) {
	std::cout << "arrive to headers" << std::endl;
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
