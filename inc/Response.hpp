#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "WebServ.hpp"

class Response {

	private:
		int									_response_code;
		std::map<std::string, std::string>	_headers;
		std::string							_content;
		std::string							_final_reply;

		void 		buildResponse(void);

	public:
		Response(void);
		Response(Client client);
		~Response(void);
		Response(const Response& src);


		std::string getFinalReply(void) const;

		
};

#endif