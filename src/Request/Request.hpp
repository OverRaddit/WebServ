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

	void	setReqBody(string req_body);
	void	setReqHeader(string key, string value);

	string	getReqHeaderValue(string key);
	string	getMethod(void) const;
	string	getReqTarget(void) const;
	string	getHttpVersion(void) const;
	string	getReqBody(void) const;
private:
	map<string, string>	m_req_header;
	string				m_method;
	string				m_req_target;
	string				m_http_version;
	string				m_req_body;

	Request(void);

	void	saveStartLine(string start_line);
	void	saveHeader(string header_line);
};

#endif