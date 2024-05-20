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

void Config::loadFromFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw Except("Could not open file: " + filename);
    }

    std::string line;
    Server current_server;
    bool in_server_block = false;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "server") {
            if (in_server_block) {
                _servers.push_back(current_server);
                current_server = Server();
            }
            in_server_block = true;
            parseServerBlock(file, current_server);
            in_server_block = false;
        }
    }

    if (in_server_block) {
        _servers.push_back(current_server);
    }
}

void Config::parseServerBlock(std::ifstream &file, Server &server) {
    std::string line;
    bool in_location_block = false;
    Location current_location;

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
        } else if (token == "root" && !in_location_block) {
            iss >> server.root;
        } else if (token == "index") {
            iss >> server.index;
        } else if (token == "error_page") {
            iss >> server.error_page;
        } else if (token == "client_max_body_size") {
            iss >> server.client_max_body_size;
        } else if (token == "location") {
            if (in_location_block) {
                server.locations[current_location.path] = current_location;
                current_location = Location();
            }
            iss >> current_location.path;
            in_location_block = true;
            parseLocationBlock(file, current_location);
            in_location_block = false;
            server.locations[current_location.path] = current_location;
        } else if (token == "}") {
            break;
        }
    }
}

void Config::parseLocationBlock(std::ifstream &file, Location &location) {
    std::string line;

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
            location.autoindex = (value == "on");
        } else if (token == "}") {
            break;
        }
    }
}

void Config::printConfig() const {
    for (std::vector<Server>::const_iterator it = _servers.begin(); it != _servers.end(); ++it) {
        const Server& server = *it;
        std::cout << "Server listening on ports: ";
        for (std::vector<int>::const_iterator port_it = server.listen_ports.begin(); port_it != server.listen_ports.end(); ++port_it) {
            std::cout << *port_it << " ";
        }
        std::cout << std::endl;

        std::cout << "Server name: " << server.server_name << std::endl;
        std::cout << "Root directory: " << server.root << std::endl;
        std::cout << "Index file: " << server.index << std::endl;
        std::cout << "Error page: " << server.error_page << std::endl;
        std::cout << "Max body size: " << server.client_max_body_size << " bytes" << std::endl;

        for (std::map<std::string, Location>::const_iterator loc_it = server.locations.begin(); loc_it != server.locations.end(); ++loc_it) {
            const Location& loc = loc_it->second;
            std::cout << "Location path: " << loc.path << std::endl;
            std::cout << "Allowed methods: ";
            for (std::vector<std::string>::const_iterator method_it = loc.allow_methods.begin(); method_it != loc.allow_methods.end(); ++method_it) {
                std::cout << *method_it << " ";
            }
            std::cout << std::endl;
            std::cout << "Root: " << loc.root << std::endl;
            std::cout << "Upload store: " << loc.upload_store << std::endl;
            std::cout << "CGI pass: " << loc.cgi_pass << std::endl;
            std::cout << "Autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
        }
    }
}

void Config::parseConfigFile(const std::string &filename) {
		loadFromFile(filename);
		if (DEBUG) {
			std::cout << GRN << "Config file parsed successfully" << CLR << std::endl;
			printConfig();
		}
}

const std::vector<Server>& Config::getServers() const {
    return _servers;
}
