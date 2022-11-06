#include "ServerBlock.hpp"

ServerBlock::ServerBlock(void) {}

void	ServerBlock::setLocationBlock(string loc_block)
{
	size_t	pos;
	string	route = "";

	pos = loc_block.find("/");
	for (size_t i = pos;loc_block[i] != ' ';i++)
		route += loc_block[i];
	this->m_loc_blocks[route] = LocationBlock();
	if ((pos = loc_block.find("limit_except")) != string::npos)
		this->m_loc_blocks[route].setValidMethod(loc_block, pos);
	if ((pos = loc_block.find("client_max_body_size")) != string::npos)
		this->m_loc_blocks[route].setMaxBodySize(loc_block, pos);
	if ((pos = loc_block.find("\tupload")) != string::npos)
		this->m_loc_blocks[route].setUploadDirectory(loc_block, pos);
	if ((pos = loc_block.find("\tautoindex")) != string::npos)
		this->m_loc_blocks[route].setAutoIndex(loc_block, pos);
	if ((pos = loc_block.find("root")) != string::npos)
		this->m_loc_blocks[route].setRootDir(loc_block, pos);
	if ((pos = loc_block.find("return")) != string::npos)
		this->m_loc_blocks[route].setRedirectionURL(loc_block, pos);
	if ((pos = loc_block.find("type")) != string::npos)
		this->m_loc_blocks[route].setRequestType(loc_block, pos);
}

void	ServerBlock::setErrorPage(string error_page_info)
{
	string			status_code = "";
	string			error_page_path = "";
	int				int_status_code;
	bool			flag = true;

	for (int i = 0;i < error_page_info.length();i++)
	{
		if (error_page_info[i] == ' ')
		{
			flag = false;
			continue;
		}
		if (flag)
			status_code += error_page_info[i];
		else
			error_page_path += error_page_info[i];
	}
	// 에러 페이지 경로 예외처리..?
	this->m_error_page[stoi(status_code)] = error_page_path;
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

string	ServerBlock::getErrorPage(int error_code) const { return this->m_error_page.at(error_code); }

string ServerBlock::getIndexFile(void) const { return this->m_index_file; }

string ServerBlock::getCgiTester(void) const { return this->m_cgi_tester; }

string ServerBlock::getCgiExtension(void) const { return this->m_cgi_extension; }

string ServerBlock::getRootDir(void) const { return this->m_root_dir; }

string ServerBlock::getServerName(void) const { return this->m_server_name; }

int	ServerBlock::getPortNum(void) const { return this->m_port_num; }

const map<string, LocationBlock>&	ServerBlock::getLocationBlocks(void) const {
	return this->m_loc_blocks;
}

