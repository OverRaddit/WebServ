#include "Request.hpp"
#include <cstddef>
#include <string>

Request::Request() {}

Request::Request(const Request& a)
{
	*this = a;
}

Request& Request::operator=(const Request& a)
{
	m_req_header = a.m_req_header;
	m_http_version = a.m_http_version;
	m_method = a.m_method;
	m_req_body = a.m_req_body;
	m_req_target = a.m_req_target;

	return *this;
}

Request::~Request(){}


Request::Request(string request_msg): m_req_header(), m_http_version(""), m_method(""), m_req_body(""), m_req_target(""), m_content_length(0), m_req_type(2), m_del_file_name("")
{
	string	line = "";

	for (int i = 0;request_msg[i];i++)
	{
		if (request_msg[i] == '\r')
		{
			if (m_method.empty())
				this->saveStartLine(line);
			else if (line.length() == 0)
			{
				this->setReqBody(request_msg.substr(i + 2));
				break;
			}
			else
				this->saveHeader(line);
			line = "";
		}
		else if (request_msg[i] != '\n')
			line += request_msg[i];
	}
}

int		Request::saveOnlyBody(string req_body)
{
	this->m_req_body.append(req_body);
	return req_body.length();
}

void	Request::saveStartLine(string start_line)
{
	int		cnt = 0;
	size_t	pos;

	for (int i = 0;i < start_line.length();i++)
	{
		if (start_line[i] != ' ')
		{
			if (cnt == 0)
				this->m_method += start_line[i];
			else if (cnt == 1)
				this->m_req_target += start_line[i];
			else
				this->m_http_version += start_line[i];
		}
		else
			cnt++;
	}
	if (this->m_del_file_name == "" && this->m_method == "DELETE")
	{
		if ((pos = this->m_req_target.find("delete")) != string::npos)
		{
			for (size_t i = pos + 7;i < m_req_target.length();i++)
				this->m_del_file_name += this->m_req_target[i]; 
		}
	}
}

void	Request::saveHeader(string header_line)
{
	string	key = "";
	string	value = "";
	int		i;

	for(int i=0;i<header_line.length();i++)
	{
		if (header_line[i] != ':')
			continue ;

		this->setReqHeader(header_line.substr(0, i), header_line.substr(i+2));
		break;
	}
}

void	Request::setStatusCode(int status_code)
{
	this->m_status_code = status_code;
}

void	Request::setReqBody(string body)
{
	this->m_req_body = body;
}

void	Request::setRedirectionURL(string url)
{
	this->m_redirection_url = url;
}

void	Request::setReqHeader(string key, string value)
{
	if (key == "Content-Length" || key == "content-length")
		setContentLength(value);
	this->m_req_header.insert(pair<string, string>(key, value));
}

void	Request::setContentLength(string content_length)
{
	this->m_content_length = stoll(content_length);
}

void	Request::setReqType(int type)
{
	this->m_req_type = type;
}

long long	Request::getContentLength(void) const {
	return this->m_content_length;
}

string	Request::getReqBody(void) const {
	return this->m_req_body;
}

string	Request::getMethod(void) const {
	return this->m_method;
}

string	Request::getReqTarget(void) const {
	return this->m_req_target;
}

string	Request::getHttpVersion(void) const {
	return this->m_http_version;
}

string	Request::getRedirectionURL(void) const {
	return this->m_redirection_url;
}

int		Request::getStatusCode(void) const {
	return this->m_status_code;
}

string		Request::getDelFileName(void) const {
	return this->m_del_file_name;
}

string	Request::getReqHeaderValue(string key) {
	string	lower_key = "";
	for (int i = 0;i < key.length();i++)
		lower_key += tolower(key[i]);
	if (this->m_req_header.find(key) != this->m_req_header.end())
		return this->m_req_header[key];
	if (this->m_req_header.find(lower_key) != this->m_req_header.end())
		return this->m_req_header[lower_key];
	return "";
}

int		Request::getReqType(void) const
{
	return m_req_type;
}
