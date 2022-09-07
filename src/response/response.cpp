#include "./Response.hpp"

bool Response::initFlag = false;

Response::Response() {
	if (Response::initFlag == false)
		this.ResponseInit();
}

Response::~Response() {
}

void Response::ResponseInit() {
	this->m_status_description[200] = "200 OK";
	this->m_status_description[201] = "201 Created";
	this->m_status_description[204] = "204 No Content";
	this->m_status_description[300] = "300 Multiple Choices";
	this->m_status_description[301] = "301 Moved Permanently";
	this->m_status_description[302] = "302 Found";
	this->m_status_description[303] = "303 See Other";
	this->m_status_description[307] = "307 Temporary Redirect";
	this->m_status_description[400] = "400 Bad Request";
	this->m_status_description[404] = "404 Not Found";
	this->m_status_description[405] = "405 Method Not Allowed";
	this->m_status_description[408] = "408 Request Timeout";
	this->m_status_description[411] = "411 Length Required";
	this->m_status_description[413] = "413 Request Entity Too Large";
	this->m_status_description[414] = "414 Request-URI Too Long";
	this->m_status_description[500] = "500 Internal Server Error";
	this->m_status_description[502] = "502 Bad Gateway";
	this->m_status_description[505] = "505 HTTP Version Not Supported";
	this->initFlag = true;
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

void Response::makeStatus200(int code) {
	if (code == 200) {

	} else if (code == 201) {

	} else if (code == 202) {

	} else if (code == 203) {

	} else if (code == 204) {

	} else if (code == 205) {

	} else if (code == 206) {
	}
}
void Response::makeStatus300(int code) {
	if (code == 300) {

	} else if (code == 301) {

	} else if (code == 302) {

	} else if (code == 303) {

	} else if (code == 304) {

	} else if (code == 305) {

	} else if (code == 306) {

	} else if (code == 307) {

	}

}
void Response::makeStatus400(int code) {
	if (code == 400) {

	} else if (code == 402) {

	} else if (code == 403) {

	} else if (code == 404) {

	} else if (code == 404) {

	} else if (code == 405) {

	} else if (code == 406) {

	} else if (code == 408) {
	} else if (code == 409) {
	} else if (code == 410) {
	} else if (code == 411) {
	} else if (code == 413) {
	} else if (code == 414) {
	} else if (code == 415) {
	} else if (code == 417) {
	} else if (code == 426) {

	}

}
void Response::makeStatus500(int code) {
	if (code == 500) {

	} else if (code == 501) {

	} else if (code == 502) {

	} else if (code == 503) {

	} else if (code == 504) {

	} else if (code == 505) {

	}
}

string Response::makeHeaders() {
	string result = "";

	result.append("HTTP/1.1 " + this->m_status_code + " " + this->m_status_description[this->m_status_code] + "\r\n");
	for (map<string, string>::iterator i = this->m_headers.begin(); i != this->m_headers.end(); i++)
		result.append((*i).first + ": " + (*i).second + "\r\n");
	result.append("\r\n");

	return result;
}

string Response::makeContent() {
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
	result += this->makeBody();
	return result;
}
