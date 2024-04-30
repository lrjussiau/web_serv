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
