#include "../inc/Utils.hpp"

BlockParser::BlockParser(const std::string& inputData) : data(inputData), pos(0) {}

std::string BlockParser::getNextBlock() {
    std::string token;
    int braceDepth = 0;
    bool capture = false;

    for (; pos < data.length(); ++pos) {
        char ch = data[pos];
        if (ch == '{') {
            if (braceDepth == 0) {
                capture = true;
            }
            braceDepth++;
        }
        if (capture) {
            token += ch;
        }
        if (ch == '}') {
            braceDepth--;
            if (braceDepth == 0) {
                capture = false;
                pos++;
                break;
            }
        }
    }
	return token;
}

std::string getTime(){
	std::time_t currentTime = std::time(0);
    char buffer[80];
	std::tm* gmtTime = std::gmtime(&currentTime);

	std::strftime(buffer, 80, "%a, %d %B %Y %H:%M:%S GMT", gmtTime);
	return std::string (buffer);
}

PathType getPathType(const std::string& path) {
	struct stat pathStat;
	if (stat(path.c_str(), &pathStat) != 0) {
		return PATH_NOT_FOUND;
	}
	if (S_ISREG(pathStat.st_mode)) {
		return PATH_IS_FILE;
	}
	if (S_ISDIR(pathStat.st_mode)) {
		return PATH_IS_DIRECTORY;
	}
	return PATH_NOT_FOUND;
}
