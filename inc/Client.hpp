#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "WebServ.hpp"

class Client {

	private:
		int			_socket;
		int			_server_socket;
		/* exemple d'attributs ->louis
		std::string _request_buffer;
    	std::string _response_Buffer;
    	std::string requestedUrl;*/

	public:
		Client(void);
		Client(int server_socket, int socket);
		~Client(void);
		Client(const Client& src);

		int	getSocket(void) const;
		int	getServerSocket(void) const;
		//Client &operator=(const Client& src);

		//louis parsing + assignation private attrinute
		void	set(char buffer);
};

#endif