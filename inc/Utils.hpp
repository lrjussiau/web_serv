#ifndef UTILS_HPP
#define UTILS_HPP

#include "WebServ.hpp"

enum PathType {
    PATH_NOT_FOUND,
    PATH_IS_FILE,
    PATH_IS_DIRECTORY
};

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

class BlockParser {
private:
    std::string data;
    size_t pos;

public:
    BlockParser(const std::string& inputData);
    std::string getNextBlock();
};

std::string getTime(void);
int         hexToInt(const std::string& hexStr);
PathType    getPathType(const std::string& path);
void        setNonBlocking(int fd, int server);
std::string generateSessionId(void);
void        signalHandler(int signum);

#endif