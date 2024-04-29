#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// Define color reset
#define RESET "\033[0m"

// Regular colors
#define BLACK "\033[30m"      /* Black */
#define RED "\033[31m"        /* Red */
#define GREEN "\033[32m"      /* Green */
#define YELLOW "\033[33m"     /* Yellow */
#define BLUE "\033[34m"       /* Blue */
#define MAGENTA "\033[35m"    /* Magenta */
#define CYAN "\033[36m"       /* Cyan */
#define WHITE "\033[37m"      /* White */

// Bold
#define BOLD "\033[1m"      /* Bold Black */

#include <string>
#include <vector>
#include <map>

#include <iostream>

#include "Config.hpp"

# ifndef DEBUG
#  define DEBUG 0
# endif

#endif