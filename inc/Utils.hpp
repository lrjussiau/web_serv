#ifndef UTILS_HPP
#define UTILS_HPP

#include "WebServ.hpp"

class Except : public std::exception {
private:
    std::string message;
public:
    Except(const std::string& msg) : message(msg) {}

    virtual ~Except() throw() {} 
	
    const char* what() const throw() {
        return message.c_str();
    }
};

#endif

