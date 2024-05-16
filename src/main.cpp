#include "../inc/WebServ.hpp"

int main(int argc, char **argv) {
	
	try {
		if (argc != 2)
        	throw Except("Please provide a config file. ./web_serv \"file\"");
		Config Test;
		Test.parseConfigFile(argv[1]);
    } catch (const Except& e) {
        std::cout << RED << "Caught an exception: " << e.what() << RST << std::endl;
    }
}
