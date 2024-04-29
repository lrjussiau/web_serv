#include "../inc/WebServ.hpp"

int main() {
	std::cout << "lol" << std::endl;

	if (DEBUG)
		std::cout << BOLD << CYAN << "DEBUG" << RESET << std::endl;

	try {
        throw Except("test");
    } catch (const Except& e) {
        std::cout << BOLD << RED << "Caught an exception: " << e.what() << RESET << std::endl;
    }
}