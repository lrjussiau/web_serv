#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Client.hpp"

// split server launch?
class Server {

	private:
		std::vector<int>			_sockets;
		std::map<int, Client>		_clients;

	public:
		Server(void);
		virtual ~Server();
		Server(const Server& src);
		Server &operator=(const Server& src);


		std::vector<int>	getSockets(void) const;
		std::vector<int>	getClients(void) const;
		Client				getClient(int socket_client) const;
		void				addClient(int client_socket, int server_socket);
		void				setClient(int client_socket, char buffer);
		//void				createServer(ServerConfig server_config);
		int					launchSocket(uint16_t port, const char *ip, bool IPv4);
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