#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include <map>
# include "LocationBlock.hpp"

using namespace std;

class ServerBlock {
public:
	ServerBlock();
private:
	int		m_port_num;
	string	m_server_name;
	string	m_root_dir;
	string	m_cgi_tester;
	map<int, string>	m_error_page;
	map<string, LocationBlock>	m_loc_blocks;
};

#endif