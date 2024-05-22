#ifndef SUPERVISOR_HPP
#define SUPERVISOR_HPP

#include "Server.hpp"
#include "Config.hpp"
#include "Client.hpp"
#include "Response.hpp"
// #include "WebServ.hpp"

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
		std::map<int, Server*>					_servers_map;
		std::map<int, Client>					_clients_map;

	public:
		Supervisor(void);
		~Supervisor(void);
		Supervisor(const Supervisor& src);


		//void	fdSetAdd(int socket_fd);
		void	fdSetRemove(int socket_fd);
		int		isServer(int socket_fd) const;
		void	shutdown(void);
		void	buildServers(Config configuration);
		void	addServer(ServerConfig server_config);
		void	removeServer(int fd);
		void	removeClients(int server_fd);
		void	closeClient(int client_fd);
		void	acceptNewConnection(int server_socket);
		void	readRequestFromClient(int client_socket);
		void	writeResponseToClient(int client_socket);
		void	manageOperations(void);
};

int	findFdMax(fd_set all_sockets);

#endif