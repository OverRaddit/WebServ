#include "Config.hpp"

Config::Config(void) {}

Config::Config(string file) {
	string	line = "";

	this->m_config_file.open(file);
	if (this->m_config_file.fail())
	{
		cerr << "Error\n";
		exit(1);
	}
	while (getline(this->m_config_file, line))
	{
		
	}
}
