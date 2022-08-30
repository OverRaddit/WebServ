#include "Request.hpp"

Request::Request() {}

Request::Request(string request_msg): m_req_header(), m_http_version(""), m_method(""), m_req_body(""), m_req_target("")
{
	string	line = "";

	for (int i = 0;request_msg[i];i++)
	{
		if (request_msg[i] == '\r')
		{
			if (m_method.empty())
				this->saveStartLine(line);
			else if (line.length() == 0)
				this->setReqBody(request_msg.substr(i + 2));
			else
				this->saveHeader(line);
			line = "";
		}
		else if (request_msg[i] != '\n')
			line += request_msg[i];
	}
}


void	Request::saveStartLine(string start_line)
{
	int	cnt = 0;

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
}

void	Request::saveHeader(string header_line)
{
	string	key = "";
	string	value = "";
	int		flag = 0;
	int		i;

	for (i = 0;i < header_line.length();i++)
	{
		if (header_line[i] == ':')
		{
			flag = 1;
			i++;
			continue ;
		}
		if (flag)
			value += header_line[i];
		else
			key += header_line[i];
	}
	this->setReqHeader(key, value);
}

void	Request::setReqBody(string body)
{
	this->m_req_body = body;
}

void	Request::setReqHeader(string key, string value)
{
	this->m_req_header.insert(pair<string, string>(key, value));
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

string	Request::getReqHeaderValue(string key) {
	return this->m_req_header[key];
}