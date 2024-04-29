#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>

class Config {
private:
    std::string					_host;
    int 						_port;
    std::string 				_server_name;
    bool 						_default_server;
    std::map<int, std::string>	_error_pages;
    size_t						_max_body_size;

    struct RouteConfig {
        std::string 				path;
        std::string 				root;
        std::string 				default_file;
        bool 						directory_listing;
        std::vector<std::string> 	allowed_methods;
        bool 						cgi_enabled;
        std::string 				cgi_handler;
        std::vector<std::string> 	file_extensions;
        bool 						upload_enabled;
        std::string					save_path;
    };

    std::vector<RouteConfig> _routes;

public:
    Config(const std::string& configFile);
	virtual ~Config();
	Config(const Config& src);
	Config &operator=(const Config& src);

    void parseConfigFile();

    std::string 						getHost() const;
    int 								getPort() const;
    std::string 						getServerName() const;
    bool 								isDefaultServer() const;
    std::map<int, std::string>			getErrorPages() const;
    size_t 								getMaxBodySize() const;
    const std::vector<RouteConfig>& 	getRoutes() const;
};

#endif