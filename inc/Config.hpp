#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "WebServ.hpp"

struct Location {
    std::string                 path;
    std::vector<std::string>    allow_methods;
    std::string                 root;
    std::string                 upload_store;
    std::string                 cgi_pass;
    bool                        autoindex;
};

struct ServerConfig {
    std::vector<int>                listen_ports;
    std::string                     server_name;
    bool                            is_ipv4;
    std::string                     root;
    std::string                     index;
    std::map<int, std::string>    	error_pages;
    int                             client_max_body_size;
    std::map<std::string, Location> locations;
};

class Config {

    private:
        std::vector<ServerConfig> _servers;

        void		parseServerBlock(std::ifstream &file, ServerConfig &server);
        Location	parseLocationBlock(std::ifstream &file);
		void		loadFromFile(const std::string &filename);
		void		printConfig() const;
		int			parseSize(const std::string &size_str);

    public:
		Config();
		Config(const Config &src);
		virtual ~Config();
		Config &operator=(const Config &src);

		void parseConfigFile(const std::string &filename);
        const std::vector<ServerConfig>& getServers() const;

};

#endif