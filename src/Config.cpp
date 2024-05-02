#include "../inc/Config.hpp"

Config::Config() {};

Config::~Config() {};

Config::Config(const Config &src) {
	(void) src;
}

Config &Config::operator=(const Config &src) {
	(void) src;
	return *this;
}

//---------------------------------------//
//             	   Utils                 //
//---------------------------------------//

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

std::string getValue(std::string line) {
	return (trim(line.substr(line.find(":") + 1)));
}

void Config::printConfig() {
	int i = 0;

	std::cout << CYA << "------------------------------------" << std::endl;
	std::cout << "           Config data" << std::endl;
	std::cout << "------------------------------------" << RST << std::endl;
	for (std::vector<ServerConfig>::iterator it = _config.begin(); it != _config.end(); ++it) {
		i++;
		std::cout << std::endl << GRN << "server " << i << " :" << RST << std::endl;
    	std::cout << "host : " << it->_host << std::endl;
		std::cout << "port : " << it->_port << std::endl;
		std::cout << "server_name : " << it->_server_name << std::endl;
		if (it->_default_server)
			std::cout << "default_server : true" << std::endl;
		else
			std::cout << "default_server : false" << std::endl;
		std::cout << "error_pages :" << std::endl;
		for (std::map<int, std::string>::iterator ite = it->_error_pages.begin(); ite != it->_error_pages.end(); ++ite) {
			std::cout << "\t" << ite->first << " : " << ite->second << std::endl;
		}
		std::cout << "client_max_body_size : " << it->_max_body_size << std::endl << std::endl;
		if (!it->_routes.empty()) {
			std::cout << "route : " << std::endl;
			for (std::vector<RouteConfig>::iterator ite = it->_routes.begin(); ite != it->_routes.end(); ++ite) {
				std::cout << "\t" << "path : " << ite->_path << std::endl;
				std::cout << "\t" << "root : " << ite->_root << std::endl;
				std::cout << "\t" << "default_file : " << ite->_default_file << std::endl;
				if (ite->_directory_listing)
					std::cout << "\t" << "directory_listing : true" << std::endl;
				else
					std::cout << "\t" << "directory_listing : false" << std::endl;
				std::cout << "\t" << "allowed_methods :" << std::endl;
				for (std::vector<std::string>::iterator iten = ite->_allowed_methods.begin(); iten != ite->_allowed_methods.end(); ++iten)
					std::cout << "\t" << "\t" << *iten << std::endl;
				if (ite->_cgi_enabled) {
					std::cout << "\t" << "handler : " << ite->_cgi_handler << std::endl;
					std::cout << "\t" << "file_extensions :" << std::endl;
					for (std::vector<std::string>::iterator iten = ite->_file_extensions.begin(); iten != ite->_file_extensions.end(); ++iten)
						std::cout << "\t" << "\t" << *iten << std::endl;
				}
				if (ite->_upload_enabled) {
					std::cout << "\t" << "save_path : " << ite->_save_path << std::endl;
				}
			}
		}
	}
	std::cout << CYA << "------------------------------------" << RST << std::endl;
}

//---------------------------------------//
//             	  Parsing                //
//---------------------------------------//

void	parseRoute(std::string line, RouteConfig &route) {
	if (line.find("save_path:") != std::string::npos) {
        route._save_path = getValue(line);
		route._upload_enabled = true;
	} else if (line.find("path:") != std::string::npos) {
        route._path = getValue(line);
    } else if (line.find("root:") != std::string::npos) {
        route._root = getValue(line);
    } else if (line.find("default_file:") != std::string::npos) {
        route._default_file = getValue(line);
    } else if (line.find("directory_listing:") != std::string::npos) {
        if (getValue(line) == "on")
            route._directory_listing = true;
		else
			route._directory_listing = false;
	} else if (line.find("allowed_methods:") != std::string::npos) {
		std::stringstream ss(getValue(line));
		std::string token;

		while (getline(ss, token, ',')) {
			route._allowed_methods.push_back(trim(token));
		}
	} else if (line.find("handler:") != std::string::npos) {
        route._cgi_handler = getValue(line);
		route._cgi_enabled = true;
	} else if (line.find("file_extensions:") != std::string::npos) {
		std::stringstream ss(getValue(line));
		std::string token;

		while (getline(ss, token, ',')) {
			route._file_extensions.push_back(trim(token));
		}
	}
}

void	parseServer(std::string line, ServerConfig &server) {
    if (line.find("host:") != std::string::npos) {
        server._host = getValue(line);
    } else if (line.find("port:") != std::string::npos) {
		try {
            server._port = std::stoul(getValue(line));
		} catch (...) {
			throw Except("port must be a unsigned int");
		}
     } else if (line.find("server_name:") != std::string::npos) {
        server._server_name = getValue(line);
	} else if (line.find("default_server:") != std::string::npos) {
        if (getValue(line) == "true")
        	server._default_server = true;
		else
			server._default_server = false;
	} else if (line.find("client_max_body_size:") != std::string::npos) {
		try {
        	server._max_body_size = std::stol(getValue(line));
		} catch (...) {
			throw Except("client_max_body_size must be a unsigned int");
		}
	} else if (line.find("error_pages:") != std::string::npos) {
		std::stringstream 	ss(getValue(line));
		std::string			token;

		while (getline(ss, token, ',')) {
			std::stringstream pairStream(token);
			int code;
			std::string path;

			pairStream >> code;
			pairStream >> path;
			if (path.empty() || !code)
				throw Except("Error page must have a int and a path");
			server._error_pages[code] = path;
		}
	}
}

void	Config::parseBlock(std::string block) {
	std::istringstream iss(block);
    std::string line;
    ServerConfig server;
    RouteConfig route;
    bool inRoute = false;
	int	bracet = 1;

	while (getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        if (line.find("route {") != std::string::npos) {
            inRoute = true;
            continue;
		} else if (line.find("}") != std::string::npos && inRoute && bracet == 0) {
		    server._routes.push_back(route);
            route = RouteConfig();
            inRoute = false;
            continue;
        } else if (line.find("{") != std::string::npos && inRoute) {
            bracet++;
            continue;
		} else if (line.find("}") != std::string::npos && inRoute) {
            bracet--;
            continue;
        }
        if (inRoute) {
			parseRoute(line, route);
        } else {
			parseServer(line, server);
        }
    }
	_config.push_back(server);
}

void	Config::separateServer(std::string content) {
    BlockParser parser(content);
    std::string block;

    while (!(block = parser.getNextBlock()).empty()) {
		parseBlock(block);
    }
}

void	Config::checker() {
	for (std::vector<ServerConfig>::iterator it = _config.begin(); it != _config.end(); ++it) {
		if (!it->_port)
			throw Except("port is missing, please provide a int beetween 0 and 65535");
		if (it->_port > 65535 || it->_port < 0)
			throw Except("port must be beetween 0 and 65535");
		if (it->_host.empty())
			throw Except("host is missing, please provide the host adress");
		for (std::vector<RouteConfig>::iterator ite = it->_routes.begin(); ite != it->_routes.end(); ++ite) {
			if (ite->_path.empty())
				throw Except("you need to specified a path");
			if (ite->_path[0] != '/')
				throw Except("the path must start by /");
			if (ite->_root.empty())
				throw Except("you need to specified a root");
			if (ite->_root[0] != '/')
				throw Except("the root must start by /");
			if (ite->_default_file.empty())
				throw Except("you need to specified a default file");
			if (ite->_cgi_handler.empty() && ite->_cgi_enabled)
				throw Except("you need to specified a cgi handler");
			if (ite->_cgi_handler[0] != '/' && ite->_cgi_enabled)
				throw Except("the CGI handler must start by /");
			if (ite->_save_path.empty() && ite->_upload_enabled)
				throw Except("you need to specified a save_path");
			if (ite->_save_path[0] != '/' && ite->_upload_enabled)
				throw Except("the save path must start by /");
		}
	}
}

void Config::parseConfigFile(const std::string& configFile) {
	std::ifstream file(configFile);
	if (!file.is_open()) {
		throw Except("can not open file.");
	}
	std::string 		line;
	std::string 		content;
	while (std::getline(file, line)) {
		content += line + "\n";
	}
	separateServer(content);
	// if (DEBUG)
		printConfig();
	checker();
}


// Check if each serv not same name / port / host