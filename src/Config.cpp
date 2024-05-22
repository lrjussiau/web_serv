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
// 					  Utils Functons
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

bool isStringDigit(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isdigit(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    return true;
}

std::string get_final_page(std::string page, int code) {
    std::istringstream iss(page);
    std::string token;
    std::vector<std::string> parts;
    
    while (std::getline(iss, token, '/')) {
        parts.push_back(token);
    }
    if (!parts.empty()) {
        std::string& lastPart = parts.back();
        size_t pos;
        while ((pos = lastPart.find('x')) != std::string::npos) {
            lastPart.replace(pos - 2, 3, std::to_string(code));
        }
    }
    std::string final_page;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i != 0) {
            final_page += '/';
        }
        final_page += parts[i];
    }
    
    return final_page;
}


void    Config::parseErrorPage(std::ifstream &file, ServerConfig &server, std::string line) {
    std::vector<int> error_codes;
    std::vector<std::string> error_pages;
    (void)server;

    do {
        if (line == "")
            break;
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        do {
            if (isStringDigit(token)) {
                error_codes.push_back(std::stoi(token));
            } else {
                error_pages.push_back(token);
            }
        } while (iss >> token);
    } while (std::getline(file, line));

    for (std::vector<std::string>::iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
        if (it->find("50") != std::string::npos) {
            for (std::vector<int>::iterator it2 = error_codes.begin(); it2 != error_codes.end(); ++it2) {
                if (*it2 >= 500 && *it2 <= 599) {
                    server.error_pages[*it2] = get_final_page(*it, *it2);
                }
            }
        } else if (it->find("40") != std::string::npos) {
            for (std::vector<int>::iterator it2 = error_codes.begin(); it2 != error_codes.end(); ++it2) {
                if (*it2 >= 400 && *it2 <= 499) {
                    server.error_pages[*it2] = *it;
                }
            }
        } else if (it->find("30") != std::string::npos) {
            for (std::vector<int>::iterator it2 = error_codes.begin(); it2 != error_codes.end(); ++it2) {
                if (*it2 >= 300 && *it2 <= 399) {
                    server.error_pages[*it2] = *it;
                }
            }
        }
    }
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
            while (iss >> port) {
                server.listen_ports.push_back(port);
            }
        } else if (token == "server_name") {
            iss >> server.server_name;
            if (checkIpv4(server)) {
                server.is_ipv4 = true;
            } else {
                // if (!checkIpv6(server)) {
                //     throw Except("Invalid server_name: " + server.server_name);
                // }
            }
        } else if (token == "root") {
            iss >> server.root;
        } else if (token == "index") {
            iss >> server.index;
        } else if (token == "error_page") {
            parseErrorPage(file, server, line);
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

// ------------------------------------------------------
// 					 Check IP type
// ------------------------------------------------------

bool Config::checkIpv4(ServerConfig &server) const {
    std::vector<std::string> octets;
    std::string octet;
    std::istringstream iss(server.server_name);
    while (std::getline(iss, octet, '.')) {
        octets.push_back(octet);
    }
    if (octets.size() != 4) {
        return false;
    }
    for (std::vector<std::string>::const_iterator it = octets.begin(); it != octets.end(); ++it) {
        if (it->size() > 3) {
            return false;
        }
        for (std::string::const_iterator c = it->begin(); c != it->end(); ++c) {
            if (!std::isdigit(*c) && *c != ';'){
                return false;
            }
        }
        if (it->empty() || (it->back() == ';' && it->size() == 1)) {
            return false;
        }
        int octet = std::stoi(*it);
        if (octet < 0 || octet > 255) {
            return false;
        }
    }
    return true;
}

bool isValidHexDigit(char c) {
    return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

bool isValidIPv6Group(const std::string& group) {
    if (group.empty() || group.length() > 4) {
        return false;
    }
    for (std::size_t i = 0; i < group.length(); ++i) {
        if (!isValidHexDigit(group[i])) {
            return false;
        }
    }
    return true;
}

// bool Config::isValidIPv6(const std::string& address) {
//     std::vector<std::string> groups;
//     std::string currentGroup;
//     bool doubleColonFound = false;
//     std::size_t groupCount = 0;

//     for (std::size_t i = 0; i < address.length(); ++i) {
//         if (address[i] == ':') {
//             if (i > 0 && address[i-1] == ':') {
//                 if (doubleColonFound) {
//                     return false;  // More than one "::" found
//                 }
//                 doubleColonFound = true;
//                 currentGroup.clear();
//             } else {
//                 if (!currentGroup.empty()) {
//                     if (!isValidIPv6Group(currentGroup)) {
//                         return false;
//                     }
//                     groups.push_back(currentGroup);
//                     currentGroup.clear();
//                     ++groupCount;
//                 }
//             }
//         } else {
//             currentGroup += address[i];
//         }
//     }
//     if (!currentGroup.empty()) {
//         if (!isValidIPv6Group(currentGroup)) {
//             return false;
//         }
//         groups.push_back(currentGroup);
//         ++groupCount;
//     }
//     if (groupCount > 8 || (groupCount < 8 && !doubleColonFound)) {
//         return false;
//     }
//     return true;
// }

// ------------------------------------------------------
// 					 Check Config
// ------------------------------------------------------

void Config::checkConfig() const {
    for (std::vector<ServerConfig>::const_iterator it = _servers.begin(); it != _servers.end(); ++it) {
        ServerConfig server = *it;
        for (std::vector<int>::const_iterator port_it = server.listen_ports.begin(); port_it != server.listen_ports.end(); ++port_it) {
            if (*port_it == 0) {
                throw Except("Invalid port number: You have to specify a port number");
            }
            if (*port_it < 0 || *port_it > 65535) {
                throw Except("Invalid port number: " + std::to_string(*port_it));
            }
        }
        if (server.server_name.empty()) {
            throw Except("Server block missing server_name directive");
        }
        if (server.root.empty()) {
            throw Except("Server block missing root directive");
        }
        if (server.index.empty()) {
            throw Except("Server block missing index directive");
        }
        if (server.error_pages.empty()) {
            throw Except("Server block missing error_page directive");
        }
        else {
            for (std::map<int, std::string>::const_iterator ep_it = server.error_pages.begin(); ep_it != server.error_pages.end(); ++ep_it) {
                if (ep_it->first < 300 || ep_it->first > 599) {
                    throw Except("Invalid error code: " + std::to_string(ep_it->first));
                } else if (ep_it->second.empty()) {
                    throw Except("Server block missing error_page directive");
                }
            }
        }
        if (server.client_max_body_size == 0) {
            throw Except("Server block missing client_max_body_size directive");
        }
        // for (std::map<std::string, Location>::const_iterator loc_it = server.locations.begin(); loc_it != server.locations.end(); ++loc_it) {
        //     const Location& loc = loc_it->second;
        //     if (loc.allow_methods.empty()) {
        //         throw Except("Location block missing allow_methods directive");
        //     }
        //     if (loc.root.empty()) {
        //         throw Except("Location block missing root directive");
        //     }
        //     if (loc.upload_store.empty()) {
        //         throw Except("Location block missing upload_store directive");
        //     }
        //     if (loc.cgi_pass.empty()) {
        //         throw Except("Location block missing cgi_pass directive");
        //     }
        // }
    }
}

// ------------------------------------------------------
// 					 Run Config
// ------------------------------------------------------

void Config::parseConfigFile(const std::string &filename) {
		loadFromFile(filename);
		if (DEBUG) {
			std::cout << GRN << "Config file parsed successfully" << RST << std::endl;
			std::cout << BLU << "----------------------------- << Config << -----------------------------" << std::endl;
			printConfig();
			std::cout << "------------------------------------------------------------------------"<< RST << std::endl;
		}
        checkConfig();
}

// ------------------------------------------------------
// 					    Getters
// ------------------------------------------------------


const std::vector<ServerConfig>& Config::getServers() const {
    return _servers;
}
