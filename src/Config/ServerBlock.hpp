#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include <map>
# include "LocationBlock.hpp"

using namespace std;

class ServerBlock {
public:
	ServerBlock(void);

	void	setPortNum(string port_num);
	void	setServerName(string server_name);
	void	setRootDir(string root_dir);
	void	setCgiTester(string cgi_tester);
	void	setErrorPage(string error_page_info);
	void	setLocationBlock(string loc_block);
	void	setIndexFile(string file_name);

	int		getPortNum(void) const;
	string	getErrorPage(int error_code) const;
	string	getServerName(void) const;
	string	getRootDir(void) const;
	string	getCgiTester(void) const;
	string	getIndexFile(void) const;
	map<string, LocationBlock>	getLocationBlocks(void) const;
private:
	map<string, LocationBlock>	m_loc_blocks;
	map<int, string>			m_error_page;
	int							m_port_num;
	string						m_server_name;
	string						m_root_dir;
	string						m_cgi_tester;
	string						m_index_file;
};

#endif