#include "../inc/WebServ.hpp"
#include "../inc/Config.hpp"
#include "../inc/Utils.hpp"
#include "../inc/Supervisor.hpp"


#include <unistd>

std::atomic<bool> running (true);

int main(int argc, char **argv) {

    sleep(10);

    signal(SIGINT, signalHandler);
	try {
        Config config;
        Supervisor supervisor;
		if (argc > 2)
        	throw Except("Please use this configuration : ./web_serv \"file\"");
        if (argc == 1)
            config.parseConfigFile("config/default.conf");
        else
            config.parseConfigFile(argv[1]);
        supervisor.runServers(config);
        supervisor.manageOperations();
    } catch (const Except& e) {
        std::cout << RED << "Caught an exception: " << e.what() << RST << std::endl;
    }
}
