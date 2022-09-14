#include "./Response.hpp"

Response::Response() {
}

map<int, string> Response::m_status_description;

Response::~Response() {
}

void Response::ResponseInit() {
	Response::m_status_description[200] = "200 OK";
	Response::m_status_description[201] = "201 Created";
	Response::m_status_description[204] = "204 No Content";
	Response::m_status_description[300] = "300 Multiple Choices";
	Response::m_status_description[301] = "301 Moved Permanently";
	Response::m_status_description[302] = "302 Found";
	Response::m_status_description[303] = "303 See Other";
	Response::m_status_description[307] = "307 Temporary Redirect";
	Response::m_status_description[400] = "400 Bad Request";
	Response::m_status_description[404] = "404 Not Found";
	Response::m_status_description[405] = "405 Method Not Allowed";
	Response::m_status_description[408] = "408 Request Timeout";
	Response::m_status_description[411] = "411 Length Required";
	Response::m_status_description[413] = "413 Request Entity Too Large";
	Response::m_status_description[414] = "414 Request-URI Too Long";
	Response::m_status_description[500] = "500 Internal Server Error";
	Response::m_status_description[502] = "502 Bad Gateway";
	Response::m_status_description[505] = "505 HTTP Version Not Supported";
}

int	Response::getStatusCode() {
	return this->m_status_code;
}
map<int ,string> Response::getStatusDesc() {
	return this->m_status_description;
}
map<string,string> Response::getHeaders() {
	return this->m_headers;
}
string Response::getContent() {
	return this->m_content;
}

void Response::setStatusCode(int code) {
	this->m_status_code = code;
}
void Response::setStatusDesc(int code, string desc) {
	this->m_status_description[code] = desc;
}
void Response::setHeaders(string key, string value) {
	this->m_headers[key] = value;
}
void Response::setContent(string content) {
	this->m_content = content;
}

void Response::setCgiResult(string ret) {
	this->m_cgiResult = ret;
}

string Response::makeHeaders() {
	string result = "";

	result.append("HTTP/1.1 " + to_string(this->m_status_code) + " " + this->m_status_description[this->m_status_code] + "\r\n");
	for (map<string, string>::iterator i = this->m_headers.begin(); i != this->m_headers.end(); i++)
		result.append((*i).first + ": " + (*i).second + "\r\n");
	result.append("\r\n");

	return result;
}

void Response::makeContent() {
	string result = "";
	result.append("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"/><title>webserv</title></head>");
	result.append("<body>");
	result.append("<h3>" + this->m_cgiResult + "</h3>");
	result.append("<p>Click <a href=\"/\">here</a> to return home.</p>");
	result.append("</body></html>");

	this->m_content.clear();
	this->m_content = result;
}

string Response::getHttpResponse() {
	string result = this->makeHeaders();
	result += this->getContent();
	return result;
}
