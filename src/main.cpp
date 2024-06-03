#include "../inc/WebServ.hpp"
#include "../inc/Config.hpp"
#include "../inc/Utils.hpp"
#include "../inc/Supervisor.hpp"

std::atomic<bool> running (true);


void handle_sigpipe(int sig) {
    std::cerr << "Caught SIGPIPE: " << sig << std::endl;
}

int main(int argc, char **argv) {

    signal(SIGPIPE, handle_sigpipe);
    signal(SIGINT, signalHandler);
	try {
		if (argc != 2)
        	throw Except("Please provide a config file. ./web_serv \"file\"");
		Config Test;
        Supervisor supervisor;
		Test.parseConfigFile(argv[1]);
        supervisor.runServers(Test);
        supervisor.manageOperations();
    } catch (const Except& e) {
        std::cout << RED << "Caught an exception: " << e.what() << RST << std::endl;
    }
}
