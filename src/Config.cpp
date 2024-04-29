#include "../inc/Config.hpp"

Config::Config() {};

Config::~Config() {};

Config::Config(const Config &src) {
	(void) src;
}

Config &Config::operator=(const Config &src) {
	(void) src;
	return *this;
}

// throw Exception("Custom exception occurred: Specific error info here");

void Config::parseConfigFile(const std::string& configFile) {
	(void) configFile;
}