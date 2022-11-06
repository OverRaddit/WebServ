#include "./Response.hpp"

bool	check_valid(const char *suffix_url)
{
	int		 i;
	struct stat buf;
	bool	ret;

	stat(suffix_url, &buf);
	if (S_ISREG(buf.st_mode))
		ret = false;
	else if (S_ISDIR(buf.st_mode))
		ret = true;
	return ret;
}

map<int, string> Response::m_status_description;
bool Response::is_init = false;

Response::Response() {
	if (Response::is_init == false)
		Response::ResponseInit();
}

Response::Response(int status) {
	if (Response::is_init == false)
		Response::ResponseInit();
	this->setStatusCode(status);
}

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

	Response::is_init = true;
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
	this->m_content.clear();
	this->m_content = content;
	this->setHeaders("Content-length", to_string(content.length()));
}

void Response::setCgiResult(string ret) {
	this->m_cgiResult = ret;
}

string Response::makeHeaders() {
	string result = "";

	result.append("HTTP/1.1 " + this->m_status_description[this->m_status_code] + "\r\n");
	for (map<string, string>::iterator i = this->m_headers.begin(); i != this->m_headers.end(); i++)
		result.append((*i).first + ": " + (*i).second + "\r\n");
	result.append("\r\n");

	return result;
}

void Response::makeContent(string content) {
	string result = "";
	result.append("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"/><title>webserv</title></head>");
	result.append("<body>");
	result.append(content);
	result.append("<p>Click <a href=\"/\">here</a> to return home.</p>");
	result.append("</body></html>");

	this->setContent(result);
}

string Response::parseHeader(string& sub_content) {
	size_t i;

	i = sub_content.find("\r\n");  // 첫줄 바운더리 끝 찾기
	sub_content = sub_content.substr(i+2);  // 첫줄 지우기
	i = sub_content.find("\r\n");  // 둘째줄 끝 읽기
	string tmp = sub_content.substr(0, i);  // 둘쨰줄 읽기
	string name, f_name = "";

	i = tmp.find("name=");  // 이름 위치 읽기
	tmp = tmp.substr(i+6); // 이름부터 끝까지 자르기
	i = tmp.find("\"");
	name = tmp.substr(0, i);  // 파일 이름 구하기

	if (tmp.find("filename=") != string::npos) {  // 파일 이름이 있는 경우
		i = tmp.find("filename=");  // 파일 이름 위치 읽기
		tmp = tmp.substr(i+10); // 파일 이름부터 끝까지 자르기
		i = tmp.find("\"");
		f_name = tmp.substr(0, i);  // 파일 이름 구하기
	}
	i = sub_content.find("\r\n\r\n");  // 헤더 끝 위치 찾기
	sub_content = sub_content.substr(i+4);  // 바디 위치부터 시작
	if (f_name != "")
		return name + "_" + f_name;
	return name;
}

string Response::getFileContent(string &sub_content, string boundary) {
	size_t i;

	i = sub_content.find(boundary);  // 바디 끝 찾기
	string body = sub_content.substr(0, i);  // 원하는 파일의 바디 저장
	sub_content = sub_content.substr(i);  // 바디 이후부터 다시 저장
	return body;
}

void Response::cgiResponse(string cgi_result) {
	this->setHeaders("Content-Type", "text/html; charset=UTF-8");
	this->makeContent(cgi_result);
}

int Response::saveFile(string content_type, string content_body) {
	ofstream writeFile;

	size_t i = content_type.find("boundary=");
	string boundary = content_type.substr(i+9);
	string sub_content = content_body;
	string file_name;
	string file_body;

	while (sub_content.find("--" + boundary + "--") != 0) {
		file_name = parseHeader(sub_content);
		file_body = getFileContent(sub_content, "--" + boundary);
		if (file_name.find("./") != string::npos)  // 지정 디렉토리 벗어나기 금지
			return -1;
		writeFile.open("./sudo/file_storage/" + file_name);
		writeFile.write(file_body.c_str(), file_body.size());
		writeFile.close();
	}
	return 0;
}

void Response::uploadResponse(string content_type, string content_body) {
	this->setHeaders("Content-Type", "text/html; charset=UTF-8");
	if (this->saveFile(content_type, content_body) == 0)
		this->makeContent("Upload Success");
	else
		this->makeContent("Upload Fail");
	std::cout << "Upload end\n";
}

int Response::serveFile(string file_path) {
	ifstream readFile;
	string data = "";
	unsigned int i = 0;
	vector<char> d;
	char buf;

	if (file_path.find("../") != string::npos)  // 지정 디렉토리 벗어나기 금지
		return -1;
	readFile.open(file_path);
	if (!readFile.is_open())
		return -1;
	while (!readFile.eof()) {
		readFile.read(&buf, sizeof(buf));
		d.push_back(buf);
	}
	data.reserve(d.size());
	for (unsigned int i=0; i<d.size(); ++i) {
		data += d[i];
	}
	readFile.close();
	this->setContent(data);
	return 0;
}

void Response::downloadResponse(string file_path) {
	this->setHeaders("Content-Disposition", "attachment; filename=\"" + file_path + "\"");
	if (this->serveFile(file_path) != 0)
		this->makeContent("Download Fail");
}

int Response::deleteFile(string file_path) {
	if (file_path.find("../") != string::npos)  // 지정 디렉토리 벗어나기 금지
		return -1;
	string full_name = URLDecoding(file_path.c_str());
	std::cout << "Decode file name: " << full_name << "\n";
	return unlink(full_name.c_str());
}

void Response::deleteResponse(string file_path) {
	this->setHeaders("Content-Type", "text/html; charset=UTF-8");
	if (deleteFile(file_path) == 0)
		this->makeContent("Delete Success");
	else
		this->makeContent("Delete Fail");
}

int Response::getFileList(vector<string>& li, const char *dir_path) {
	struct dirent	*file    = NULL;
	DIR				*dir_ptr = NULL;

	// 목록을 읽을 디렉토리명으로 DIR *를 return
	if((dir_ptr = opendir(dir_path)) == NULL)
		return -1;

	// 디렉토리의 처음부터 파일 또는 디렉토리명을 순서대로 한개씩 읽기r.
	while((file = readdir(dir_ptr)) != NULL) {
		li.push_back(file->d_name);
	}
	// open된 directory 정보를 close.

	closedir(dir_ptr);
	return 0;
}

int	Response::getRequestFile(string request_file, string dir_path) {
	int		 i;
	struct stat buf;
	bool	ret;

	if (stat((dir_path + request_file).c_str(), &buf) < 0)
		return NO_FILE; // NO FILE!!
	if (S_ISREG(buf.st_mode))
		return VALID_REQ_FILE; // 존재하는 파일 요청
	else if (S_ISDIR(buf.st_mode))
		return VALID_REQ_DIR; // 존재하는 디렉토리 요청
}

int Response::makeAutoIndex(const char *dir_path) {
	vector<string> li;
	string result = "";
	if (getFileList(li, dir_path) == -1)
		return -1;
	result.append("<ul>\n");
	for (int i=2; i<li.size(); ++i) {
		result.append("<li><a href=\"/download/" + li[i] + "\" download>");
		result.append(li[i]);
		result.append("</a>");
		result.append("</li>");
	}
	result.append("\n</ul>\n");
	this->setContent(result);
	return 0;
}

void Response::autoIndexResponse(const char *dir_path) {
	this->setHeaders("Content-Type", "text/html; charset=UTF-8");
	if (this->makeAutoIndex(dir_path) == -1)
		this->makeContent("Auto Index Fail");
}

string Response::getHttpResponse() {
	string result = this->makeHeaders();
	result += this->getContent();
	return result;
}

/*
	여기부터 비 멤버함수
*/

// 16진수 문자값을 10진수 문자값으로 변환
char hex2int(char input) {
	// 숫자
	if (input >= '0' && input <= '9') {
		return input - '0';
	}
	// 대문자
	else if (input >= 'A' && input <= 'Z') {
		return input - 'A' + 10;
	}
	// 소문자
	else if (input >= 'a' && input <= 'z') {
		return input - 'a' + 10;
	}
	return 0;
}

// 10진수 문자값을 16진수 문자값으로 변환
char int2hex(char input) {
	char hex[17] = "0123456789abcdef";
	return hex[0x0f & input];
}

// URL 인코딩 한다.
string URLEncoding(const char *pIn) {
	if (pIn == 0) {
		return 0;
	}
	string result;
	char temp = 0;
	while (*pIn != 0) {
		temp = *pIn;
		// ASCII 대소문자와 숫자인 경우만 그대로 저장
		if (std::isprint(temp) != 0) {
			result += temp;
		}
		// % 와 2자리 16진수 문자값으로 만들어 저장
		else {
			result += "%";
			result += int2hex(temp >> 4);
			result += int2hex(temp);
		}
		pIn++;
	}
	return result;
}

// URL 디코딩 한다.
string URLDecoding(const char *pIn) {
	if (pIn == 0) {
		return 0;
	}
	string result;
	char temp = 0;
	while (*pIn != 0) {
		temp = *pIn;
		// % 로 시작되면 % 빼고 2자리 16진수 문자값을 10진수 문자값으로 변경하여 저장
		if (temp == '%') {
			char buf = 0;
			pIn++;
			temp = *pIn;
			buf = hex2int(temp);

			pIn++;
			temp = *pIn;
			buf = buf << 4 | hex2int(temp);

			result += buf;
		}
		// 출력 가능한 경우 그대로 저장
		else if (std::isprint(temp) != 0) {
			result += temp;
		}
		pIn++;
	}
	return result;
}
