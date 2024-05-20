#include "../inc/Config.hpp"


// ------------------------------------------------------
// 					Canonical Form
// ------------------------------------------------------

Config::Config() {
	return;
}

Config::Config(const Config &src) {
	*this = src;
}

Config::~Config() {
	return;
}

Config &Config::operator=(const Config &src) {
	if (this != &src) {
		this->_servers = src._servers;
	}
	return *this;
}

// ------------------------------------------------------
// 					Parse Config File
// ------------------------------------------------------

int Config::parseSize(const std::string &size_str) {
    std::string numStr;
	int 		num = 0;

    for (size_t i = 0; i < size_str.size(); ++i) {
        if (std::isdigit(size_str[i])) {
            numStr += size_str[i];
        } else if (std::isalpha(size_str[i])) {
            if (!numStr.empty()) {
                num = std::stoi(numStr);
                switch (size_str[i]) {
                    case 'M':
                        num *= 1000000;
                        break;
                    case 'K':
                        num *= 1000;
                        break;
                    case 'G':
                        num *= 1000000000;
                        break;
                }
            }
        } else if (size_str[i] == ' ' && !numStr.empty()) {
            num = std::stoi(numStr);
        }
    }
	numStr.clear();
    return num;
}

void Config::loadFromFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw Except("Could not open file: " + filename);
    }

    std::string line;
    ServerConfig current_server;
    bool in_server_block = false;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "server") {
            if (in_server_block) {
                _servers.push_back(current_server);
                current_server = ServerConfig();
				in_server_block = false;
            }
            in_server_block = true;
            parseServerBlock(file, current_server);
        }
    }

    if (in_server_block) {
        _servers.push_back(current_server);
    }
}

void Config::parseServerBlock(std::ifstream &file, ServerConfig &server) {
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "listen") {
            int port;
            iss >> port;
            server.listen_ports.push_back(port);
        } else if (token == "server_name") {
            iss >> server.server_name;
        } else if (token == "root") {
            iss >> server.root;
        } else if (token == "index") {
            iss >> server.index;
        } else if (token == "error_page" || token == "500" || token == "300" || token == "400") {
            int error_code;
			if (token != "error_page") {
				server.error_pages[std::stoi(token)] = "./src/html/error_page/" + token + ".html";
			}
            while (iss >> error_code) {
                server.error_pages[error_code] = "./src/html/error_page/" + std::to_string(error_code) + ".html";
            }
        } else if (token == "client_max_body_size") {
            std::string size_str;
            iss >> size_str;
            server.client_max_body_size = parseSize(size_str);
        } else if (token == "location") {
			std::string path;
			iss >> path;
            server.locations[path] = parseLocationBlock(file);
        } else if (token == "}") {
            break;
        }
    }
}

Location Config::parseLocationBlock(std::ifstream &file) {
    std::string line;
	Location location;


    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "allow_methods") {
            std::string method;
            while (iss >> method) {
                location.allow_methods.push_back(method);
            }
        } else if (token == "root") {
            iss >> location.root;
        } else if (token == "upload_store") {
            iss >> location.upload_store;
        } else if (token == "cgi_pass") {
            iss >> location.cgi_pass;
        } else if (token == "autoindex") {
            std::string value;
            iss >> value;
            location.autoindex = (value == "on;");
        } else if (token == "}") {
            return location;
        }
    }
	return location;
}

// ------------------------------------------------------
// 					Print Config File
// ------------------------------------------------------


void Config::printConfig() const {
    for (std::vector<ServerConfig>::const_iterator it = _servers.begin(); it != _servers.end(); ++it) {
        const ServerConfig& server = *it;
        std::cout << "Server listening on ports: ";
        for (std::vector<int>::const_iterator port_it = server.listen_ports.begin(); port_it != server.listen_ports.end(); ++port_it) {
            std::cout << *port_it << " ";
        }
        std::cout << std::endl;

        std::cout << "Server name: " << server.server_name << std::endl;
        std::cout << "Root directory: " << server.root << std::endl;
        std::cout << "Index file: " << server.index << std::endl;
        std::cout << "Error pages: " << std::endl;
        for (std::map<int, std::string>::const_iterator ep_it = server.error_pages.begin(); ep_it != server.error_pages.end(); ++ep_it) {
            std::cout << "\t" << ep_it->first << ": " << ep_it->second << std::endl;
        }
        std::cout << "Max body size: " << server.client_max_body_size << " bytes" << std::endl;

        for (std::map<std::string, Location>::const_iterator loc_it = server.locations.begin(); loc_it != server.locations.end(); ++loc_it) {
            const Location& loc = loc_it->second;
            std::cout << "Location path: " << loc_it->first << std::endl;
            std::cout << "\tAllowed methods: ";
            for (std::vector<std::string>::const_iterator method_it = loc.allow_methods.begin(); method_it != loc.allow_methods.end(); ++method_it) {
                std::cout << *method_it << " ";
            }
            std::cout << std::endl;
			if (!loc.root.empty())
            	std::cout << "\tRoot: " << loc.root << std::endl;
			if (!loc.upload_store.empty())
            	std::cout << "\tUpload store: " << loc.upload_store << std::endl;
			if (!loc.cgi_pass.empty())
            	std::cout << "\tCGI pass: " << loc.cgi_pass << std::endl;
			if (loc.autoindex) 
            	std::cout << "\tAutoindex: on" << std::endl;
        }
    }
}

void Config::parseConfigFile(const std::string &filename) {
		loadFromFile(filename);
		if (DEBUG) {
			std::cout << GRN << "Config file parsed successfully" << RST << std::endl;
			std::cout << BLU << "----------------------------- << Config << -----------------------------" << std::endl;
			printConfig();
			std::cout << "------------------------------------------------------------------------"<< RST << std::endl;
		}
}

// ------------------------------------------------------
// 					Getters
// ------------------------------------------------------


const std::vector<ServerConfig>& Config::getServers() const {
    return _servers;
}
