#include "ServerBlock.hpp"

ServerBlock::ServerBlock(void) {}

void	ServerBlock::setLocationBlock(string loc_block)
{
	size_t	pos;
	string	route = "";

	pos = loc_block.find("/");
	for (size_t i = pos;loc_block[i] != ' ';i++)
		route += loc_block[i];
	this->m_loc_blocks[route] = LocationBlock(this->m_root_dir, this->m_index_file);
	if ((pos = loc_block.find("\tlimit_except")) != string::npos)
		this->m_loc_blocks[route].setValidMethod(loc_block, pos + 1);
	if ((pos = loc_block.find("\tclient_max_body_size")) != string::npos)
		this->m_loc_blocks[route].setMaxBodySize(loc_block, pos + 1);
	if ((pos = loc_block.find("\tupload")) != string::npos)
		this->m_loc_blocks[route].setUploadDirectory(loc_block, pos + 1);
	if ((pos = loc_block.find("\tautoindex")) != string::npos)
		this->m_loc_blocks[route].setAutoIndex(loc_block, pos + 1);
	if ((pos = loc_block.find("\troot")) != string::npos)
		this->m_loc_blocks[route].setRootDir(loc_block, pos + 1);
	if ((pos = loc_block.find("\treturn")) != string::npos)
		this->m_loc_blocks[route].setRedirectionURL(loc_block, pos + 1);
	if ((pos = loc_block.find("\ttype")) != string::npos)
		this->m_loc_blocks[route].setRequestType(loc_block, pos + 1);
	if ((pos = loc_block.find("\tindex")) != string::npos)
		this->m_loc_blocks[route].setIndexFile(loc_block, pos + 1);
}

void	ServerBlock::setErrorPage(string error_page_info)
{
	this->m_error_page = error_page_info;
}


void	ServerBlock::setIndexFile(string file_name)
{
	this->m_index_file = file_name;
	// 예외처리???
}

void	ServerBlock::setCgiTester(string cgi_tester)
{
	bool flag = false;

	this->m_cgi_tester = "";
	this->m_cgi_extension = "";
	for (int i = 0;i < cgi_tester.length();i++)
	{
		if (cgi_tester[i] == ' ')
			flag = true;
		if (!flag)
			this->m_cgi_tester += cgi_tester[i];
		else
			this->m_cgi_extension += cgi_tester[i];
	}
}

void	ServerBlock::setRootDir(string root_dir)
{
	this->m_root_dir = root_dir;
	// 예외처리 필요..?
}

void	ServerBlock::setServerName(string server_name)
{
	this->m_server_name = server_name;
}

void	ServerBlock::setPortNum(string port_num)
{
	this->m_port_num = stoi(port_num);
}

string	ServerBlock::getErrorPage(void) const { return this->m_error_page; }

string ServerBlock::getIndexFile(void) const { return this->m_index_file; }

string ServerBlock::getCgiTester(void) const { return this->m_cgi_tester; }

string ServerBlock::getCgiExtension(void) const { return this->m_cgi_extension; }

string ServerBlock::getRootDir(void) const { return this->m_root_dir; }

string ServerBlock::getServerName(void) const { return this->m_server_name; }

int	ServerBlock::getPortNum(void) const { return this->m_port_num; }

const map<string, LocationBlock>&	ServerBlock::getLocationBlocks(void) const {
	return this->m_loc_blocks;
}

