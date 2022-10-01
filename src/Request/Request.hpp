#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <utility>
# include <string>
# include <map>

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

	string		getReqHeaderValue(string key);
	string		getMethod(void) const;
	string		getReqTarget(void) const;
	string		getHttpVersion(void) const;
	string		getReqBody(void) const;
	int			getStatusCode(void) const;
	long long	getContentLength(void) const;
private:
	map<string, string>	m_req_header;
	long long 			m_content_length;
	string				m_method;
	string				m_req_target;
	string				m_http_version;
	string				m_req_body;
	int					m_status_code;

	Request(void);

	void	saveStartLine(string start_line);
	void	saveHeader(string header_line);
};
void	split(const string& str, const string& delimiters , vector<string>& tokens);

#endif
