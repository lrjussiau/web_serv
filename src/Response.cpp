#include "../inc/Response.hpp"

Response::Response(void) {
	return;
}

Response::Response(Client client) {

	
	return;
}

Response::~Response(void) {
	return;
}

Response::Response(const Response& src) {
	if (this != &src)
		*this = src;
	return;
}

/*HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 19\r\n
\r\n
<html>hello</html>*/

void 		Response::buildResponse(void){
	std::string	final_reply;

	final_reply += this->_status_line.append("\r\n");
	for(std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it){
		if (it->second){
			final_reply += it->first.append(": ");
			final_reply += it->second.append("\r\n");
		}
	}
	final_reply.append("\r\n");
	final_reply += this->_content;
	this->_final_reply = final_reply;
}
