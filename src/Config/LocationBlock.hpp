#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include <iostream>
# include <string>

using namespace std;

class LocationBlock {
public:
	LocationBlock(string route);
private:
	string	m_route;
	string	m_valid_method;
	string	m_max_body_size;
	string	m_upload_dir;
	string	m_root_dir;
	bool	m_autoindex;
};

#endif