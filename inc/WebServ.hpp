
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// DEFINES
# ifndef DEBUG
#  define DEBUG 0
# endif

# define DEBUG_REPONSE 1

# define PATH_TO_REQUESTS "request_data"

// COLORS
#define RST "\033[0m"
#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define YEL "\033[1;33m"
#define BLU "\033[1;34m"
#define MAG "\033[1;35m"
#define CYA "\033[1;36m"
#define WHT "\033[1;37m"
#define ORG "\033[1;38;2;255;165;0m"
#define CLR "\033[2J\033[1;1H"

// INCLUDES
#include <vector>
#include <map>

#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <random>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netdb.h>
#include <signal.h>

extern std::atomic<bool> running;

// #include "Config.hpp"
// #include "Utils.hpp"
// #include "Client.hpp"
// #include "Server.hpp"
// #include "Supervisor.hpp"
// #include "Response.hpp"

#endif