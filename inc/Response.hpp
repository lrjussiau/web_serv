#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "WebServ.hpp"

class Response {

	private:
		std::string							_status_line;
		std::map<std::string, std::string>	_headers;
		std::string							_content;
		std::string							_final_reply;

		void 		buildResponse(void);
		void		buildStatusLine(int status_code, std::string status_message);
		void 		createContent(std::string path);	
		void		init_headers(void);
		bool		is_file_exist(const std::string& path);

	public:
		Response(void);
		Response(Client client);
		~Response(void);
		Response(const Response& src);


		std::string getFinalReply(void) const;

		
};

#endif