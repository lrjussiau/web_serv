#ifndef SERVER_HPP
#define SERVER_HPP

//#include "WebServ.hpp"
//#include "Utils.hpp"
#include "Config.hpp"
// class Config;

class Server {

	private:
		std::vector<int>			_sockets;
		ServerConfig				_server_config;
		
	public:
		Server(void);
		virtual ~Server();
		Server(const Server& src);
		Server &operator=(const Server& src);


		std::vector<int>			getSockets(void) const;
		ServerConfig				getServerConfig(void) const;
		void						createServer(ServerConfig server_config);
		int							launchSocket(unsigned port, std::string ip, bool IPv4);
		
};

class ServerSocketError : public std::exception {

	public:
		virtual const char* what() const throw() {
			return ("ServerSocketError");
		}
};

class ServerBindingError : public std::exception {

	public:
		virtual const char* what() const throw() {
			return ("ServerBindingError");
		}
};

class ServerListeningError : public std::exception {

	public:
		virtual const char* what() const throw() {
			return ("ServerListeningError");
		}
};

# endif