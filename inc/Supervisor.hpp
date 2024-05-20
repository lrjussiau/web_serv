#ifndef SUPERVISOR_HPP
#define SUPERVISOR_HPP

#include "Server.hpp"

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
		std::map<int, int>						_fd_map;
		std::map<int, Server*>					_servers_map;
		std::map<int, Client>					_clients_map;

	public:
		Supervisor(void);
		~Supervisor(void);
		Supervisor(const Supervisor& src);
		//Supervisor &operator=(const Supervisor& src); ->flemme oblige?


		void	fdSetAdd(int socket_fd);
		void	fdSetRemove(int socket_fd);
		/*int		mapFindSocketType(int socket_fd) const;
		void	mapAddElement(int socket_fd, int is_server_socket);
		void	mapRemoveElement(int socket_fd, int is_server_socket);*/
		void	addServer(ServerConfig server_config);
		void	removeServer(int fd);
		void	acceptNewConnection(int server_socket);
		void	readRequestFromClient(int client_socket);
		void	writeResponseToClient(int client_socket);
		/*Server	getServer()*/
};

int	findFdMax(fd_set all_sockets);

#endif