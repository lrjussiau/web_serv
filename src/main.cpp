#include "../inc/WebServ.hpp"

int main(int argc, char **argv) {
	
	try {
		if (argc != 2)
        	throw Except("Please provide a config file. ./web_serv \"file\"");
		Config Test;
		Test.parseConfigFile(argv[1]);
    } catch (const Except& e) {
        std::cout << BOLD << RED << "Caught an exception: " << e.what() << RESET << std::endl;
    }
}

// int main() {
//     // Simulated line input
//     std::string line = "save_path: /var/uploads";

//     // Print the line for debugging
//     std::cout << "Line: '" << line << "'" << std::endl;

//     // If block to check the condition
//     if (line.find("save_path:") != std::string::npos) {
//         std::string save_path = line.substr(line.find(":") + 1);
//         save_path = trim(save_path);
//         bool upload_enabled = true;

//         std::cout << "Save path: '" << save_path << "'" << std::endl;
//         std::cout << "Upload enabled: " << (upload_enabled ? "true" : "false") << std::endl;
//     } else {
//         std::cout << "The line does not contain 'save_path:'" << std::endl;
//     }

//     return 0;
// }