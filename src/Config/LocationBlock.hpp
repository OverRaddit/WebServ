#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include <iostream>
# include <string>
# include <sstream>

using namespace std;

class LocationBlock {
public:
	LocationBlock(void);

	void	setValidMethod(string loc_block, size_t pos);
	void	setMaxBodySize(string loc_block, size_t pos);
	void	setUploadDirectory(string loc_block, size_t pos);
	void	setRootDir(string loc_block, size_t pos);
	void	setAutoIndex(string loc_block, size_t pos);

	string	getValidMethod(void) const;
	string	getMaxBodySize(void) const;
	string	getUploadDirectory(void) const;
	string	getRootDir(void) const;
	bool	getAutoIndex(void) const;
private:
	string	m_valid_method;
	string	m_max_body_size; // int????
	string	m_upload_dir;
	string	m_root_dir;
	bool	m_autoindex;
};

#endif