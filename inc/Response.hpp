#ifndef RESPONSE_HPP
#define RESPONSE_HPP

//#include "WebServ.hpp"
//#include "Client.hpp"
#include "Config.hpp"
#include "Utils.hpp"

class Client;

class Response {

	private:
		std::string							_status_line;
		std::map<std::string, std::string>	_headers;
		std::string							_content;
		std::string							_final_reply;
		Client*								_client;
		ServerConfig						_server;

		void 		buildResponse(void);
		void 		createContent(std::string path, int status_code, std::string status_message);
		void		buildRedirectResponse(std::string redirect_path);
		void		init_headers(void);
		void		createStatusLine(int status_code, std::string status_message);

		void		handleDirectory(std::string path, Location *location);
		void		generateAutoIndex(std::string dir_requested);
		std::string generateCgi(std::string script, std::string input_string);

		bool		isMethodWrong();
		bool		isCookie(Client *client);
		bool		isCGI();
		bool		checkMimeType();
		bool		isRedirect();
		bool		checkBodySize();
		Location*	findLocation();
		std::string findPath(Location* location);
		void		fileListing(std::string directory);

	public:
		Response(void);
		Response(Client *client, ServerConfig server);
		~Response(void);
		Response(const Response& src);


		std::string getFinalReply(void) const;


		
};

#endif