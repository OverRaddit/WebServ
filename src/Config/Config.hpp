#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <fstream>
# include "ServerBlock.hpp"

using namespace std;

class Config {
public:
	Config(string file);
private:
	ifstream			m_config_file;
	vector<ServerBlock>	m_server_blocks;
	Config(void);
};

#endif