#ifndef SUPERVISOR_HPP
#define SUPERVISOR_HPP

#include "Server.hpp"
#include "Config.hpp"
#include "Client.hpp"
#include "Response.hpp"
//#include "WebServ.hpp"

// class Client;
class Config;
struct ServerConfig;
// class Server;


//->modify socket from unsigned int(can't check if < 0) to int
//manage deletion/adding of fd in the while loop or outside?

class Supervisor {

	private:
		int										_fd_max;
		fd_set									_all_sockets;
		fd_set									_read_fds;
		fd_set									_write_fds;
		fd_set									_excep_fds;
		struct timeval 							_timer;
		std::map<std::string, std::string>		_sessionIds;
		std::map<int, Server*>					_servers_map;
		std::map<int, Client>					_clients_map;

	public:
		Supervisor(void);
		~Supervisor(void);
		Supervisor(const Supervisor& src);

		void		acceptNewConnection(int server_socket);
		void		closeClient(int client_socket);
		void		closeServer(int server_socket);
		void		fdSetRemove(int socket);
		int			isServer(int socket) const;
		void		manageOperations(void);
		void		readRequestFromClient(int client_socket);
		void		removeClientsFromServer(int server_socket);
		void		runServers(Config configuration);
		void		updateFdMax(void);
		void		writeResponseToClient(int client_socket);
		/*void manageOperations(void);
    	void acceptNewConnection(int server_socket, fd_set &read_fds);
    	void readRequestFromClient(int client_socket, fd_set &read_fds, fd_set &write_fds);
    	void writeResponseToClient(int client_socket, fd_set &read_fds, fd_set &write_fds);*/

};

#endif