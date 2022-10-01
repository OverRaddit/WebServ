#include "LocationBlock.hpp"
#include <iterator>
#include <string>

LocationBlock::LocationBlock(void)
: m_max_body_size(""), m_root_dir(""), m_upload_dir(""), m_autoindex(false)
{}

void LocationBlock::setValidMethod(string loc_block, size_t pos)
{
	string	l_e = "limit_except";
	string	method = "";
	size_t	len = l_e.length();
	bool	flag = false;

	for (size_t i = pos + len + 1;loc_block[i] != ';';i++)
	{
		if (loc_block[i] != ' ')
			method += loc_block[i];
		else
			flag = true;
		if (flag || loc_block[i + 1] == ';')
		{
			this->m_valid_method.push_back(method);
			method = "";
			flag = false;
		}
	}
}

void LocationBlock::setMaxBodySize(string loc_block, size_t pos)
{
	string	l_e = "client_max_body_size";
	size_t	len = l_e.length();

	for (size_t i = pos + len + 1;loc_block[i] != ';';i++)
		this->m_max_body_size += loc_block[i];
	// int로 바꿀지 나중에 생각해보자
}

void LocationBlock::setUploadDirectory(string loc_block, size_t pos)
{
	string	l_e = "upload";
	size_t	len = l_e.length();

	for (size_t i = pos + len + 1;loc_block[i] != ';';i++)
		this->m_upload_dir += loc_block[i];
}

void LocationBlock::setRootDir(string loc_block, size_t pos)
{
	string	l_e = "root";
	size_t	len = l_e.length();

	for (size_t i = pos + len + 1;loc_block[i] != ';';i++)
		this->m_root_dir += loc_block[i];
}

void LocationBlock::setAutoIndex(string loc_block, size_t pos)
{
	string	l_e = "autoindex";
	size_t	len = l_e.length();
	string	on_off = "";

	for (size_t i = pos + len + 1;loc_block[i] != ';';i++)
		on_off += loc_block[i];
	if (on_off == "on")
		this->m_autoindex = true;
}

vector<string>	LocationBlock::getValidMethod(void) const {
	return this->m_valid_method;
}

string	LocationBlock::getMaxBodySize(void) const {
	return this->m_max_body_size;
}

string	LocationBlock::getUploadDirectory(void) const {
	return this->m_upload_dir;
}

string	LocationBlock::getRootDir(void) const {
	return this->m_root_dir;
}

bool	LocationBlock::getAutoIndex(void) const {
	return this->m_autoindex;
}
