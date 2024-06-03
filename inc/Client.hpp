#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "WebServ.hpp"

class Client {

	private:
		int			_socket;
		int			_server_socket;
		// std::string _request_buffer;
    	// std::string _response_Buffer;
    	std::string _requestedUrl;
		std::string _requestMethod;
		std::string _requestProtocol;
		std::string _requestHost;
		std::string _requestConnection;
		std::string _requestMimetype;
		std::string _postName;
		std::string _buffer;
		std::string _boundary;
		std::string	_sessionId;
		std::string	_sessionName;

		void    parseChunkedBody(std::ifstream &request_file, std::string path);
		void	parseBody(std::ifstream &request_file, std::string path);
		void	getPathToUpload(std::string line, std::string path);

	public:
		Client(void);
		Client(int server_socket, int socket, std::string sessionId);
		~Client(void);
		Client(const Client& src);
		Client &operator=(const Client& src);

		void		setData(std::string filePath);
		void		parsePostRequest(std::string path_to_request, std::string path);
		void 		setRequestedUrl(std::string url);

		int	getSocket(void) const;
		int	getServerSocket(void) const;
		std::string getRequestedUrl(void) const;
		std::string getRequestMethod(void) const;
		std::string getRequestProtocol(void) const;
		std::string getRequestHost(void) const;
		std::string getRequestConnection(void) const;
		std::string getRequestMimetype(void) const;
		std::string getPostName(void) const;
		std::string getBuffer(void) const;
		std::string	getSessionId(void) const;
		std::string	getSessionName(void) const;
		void		setSessionName(std::string session_name);
		void parseCgiPostRequest(std::string &body);
};

#endif