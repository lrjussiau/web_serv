#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "WebServ.hpp"

struct RouteConfig {
    std::string 				_path;
    std::string 				_root;
    std::string 				_default_file;
    bool 						_directory_listing;
    std::vector<std::string> 	_allowed_methods;
    bool 						_cgi_enabled;
    std::string 				_cgi_handler;
    std::vector<std::string> 	_file_extensions;
    bool 						_upload_enabled;
    std::string					_save_path;
};

/*bool	_is_IPv4;*/
struct ServerConfig {
    std::string                 _host;
    unsigned int                _port;
    std::string                 _server_name;
    bool                        _default_server;
    std::map<int, std::string>  _error_pages;
    size_t                      _max_body_size;
    std::vector<RouteConfig>    _routes;
};


class Config {
private:
    std::vector<ServerConfig> _config;

    void	separateServer(std::string content);
    void	parseBlock(std::string block);
    void    printConfig();
    void    checker();

public:
    Config();
	virtual ~Config();
	Config(const Config& src);
	Config &operator=(const Config& src);

    void    parseConfigFile(const std::string& configFile);

    std::vector<ServerConfig>   getConfig() const;
};

std::string trim(const std::string& str);

#endif