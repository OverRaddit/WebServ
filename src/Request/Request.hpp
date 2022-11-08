#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <utility>
# include <string>
# include <map>

# include "../Server/RequestType.hpp"
# include "../Config/Config.hpp"

using namespace std;

class Request {
public:
	Request(string req_msg);
	Request(const Request& a);
	Request& operator=(const Request& a);
	~Request();

	void	setContentLength(string	content_length);
	void	setReqBody(string req_body);
	void	setReqHeader(string key, string value);
	void	setStatusCode(int status_code);
	void	setRedirectionURL(string url);
	void	setReqType(int type);
	void	setCgiPid(int cgi_pid);
	void	setCgiResult(string cgi_result);
	void	setSudoDir(string sudo_dir);
	void	setLocBlock(LocationBlock loc_block);
	void	setSerBlock(ServerBlock server_block);
	void	setFilename(string file_name);

	int		saveOnlyBody(string req_body);
	void	saveRequestAgain(string req_msg);

	string				getReqHeaderValue(string key);
	string				getMethod(void) const;
	string				getReqTarget(void) const;
	string				getHttpVersion(void) const;
	string				getReqBody(void) const;
	string				getRedirectionURL(void) const;
	int					getStatusCode(void) const;
	long long			getContentLength(void) const;
	int					getReqType(void) const;
	string				getDelFileName(void) const;
	string				getDownloadFileName(void) const;
	bool				getIsIncomplete(void) const;
	string				getIncompleteMessage(void) const;
	int					getCgiPid(void) const;
	string				getCgiResult(void) const;
	string				getSudoDir(void) const;
	string				getPrefixURL(void) const;
	string				getSuffixURL(void) const;
	string				getReqFileName(void) const;
	LocationBlock		getLocBlock(void) const;
	ServerBlock			getSerBlock(void) const;

	void		saveURLInformation(void);
private:
	map<string, string>	m_req_header;
	long long 			m_content_length;
	string				m_method;
	string				m_req_target;
	string				m_http_version;
	string				m_req_body;
	int					m_status_code;
	string				m_redirection_url;
	int					m_req_type;
	string				m_del_file_name;
	string				m_file_name;
	bool				m_is_incomplete;
	string				m_incomplete_message;
	int					m_cgi_pid;
	string				m_cgi_result;
	string				m_sudo_dir; // useless
	string				m_prefix_url;
	string				m_suffix_url;
	string				m_req_file_name;

	LocationBlock		m_loc_block;
	ServerBlock			m_ser_block;

	Request(void);

	void	saveStartLine(string start_line);
	void	saveHeader(string header_line);
};

bool	is_directory(const char *suffix_url);

#endif
