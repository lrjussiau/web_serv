#ifndef RESPONSE_HPP
#define RESPONSE_HPP

class Response {

	private:
		int									_response_code;
		std::map<std::string, std::string>	_headers;
		std::string							_content;
		std::string							_response;

	
	public:
		Response(void);
		~Response(void);
		Response(const Response& src);

		
};

#endif