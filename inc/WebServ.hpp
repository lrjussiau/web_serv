#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// DEFINES
# ifndef DEBUG
#  define DEBUG 0
# endif

// COLORS
#define RST "\033[0m"
#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define YEL "\033[1;33m"
#define BLU "\033[1;34m"
#define MAG "\033[1;35m"
#define CYA "\033[1;36m"
#define WHT "\033[1;37m"
#define CLR "\033[2J\033[1;1H"

// INCLUDES
#include <string>
#include <vector>
#include <map>

#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>

#include "Config.hpp"
#include "Utils.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Supervisor.hpp"
#include "Response.hpp"

#endif