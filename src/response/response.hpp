#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include <fstream>
# include <unistd.h>

using namespace std;

class Response {
private:
	static map<int, string>	m_status_description;
	static bool				is_init;
	int						m_status_code;
	map<string, string>		m_headers;
	string					m_content;
	string					m_cgiResult;

	string		makeHeaders();
	string		parseHeader(string& content_type);
	string		getFileContent(string& content_type);

public:
	// 왜 private??
	Response ();
	Response (int status);
	~Response ();

	static void	ResponseInit(); // Response 클래스를 초기화하는 한번만 실행 가능함수
	void		makeCgiContent(string ret);
	void		makeUploadContent();

	int					getStatusCode();
	map<int ,string>	getStatusDesc();
	map<string,string>	getHeaders();
	string				getContent();

	void		setStatusCode(int code);
	void		setStatusDesc(int code, string desc);
	void		setHeaders(string key, string value);
	void		setCgiResult(string ret);
	void		setContent(string content);

	void		saveFile(string content_type, string content_body);  // m_content로 받을 데이터를 파싱해서 파일로 저장하는 함수

	void		cgiResponse(string cgi_result);  // cgi 결과를 요청하는 경우의 응답
	void		uploadResponse(string content_type, string content_body);  // 파일 업로드 경우의 응답과 처리

	string		getHttpResponse();
};

#endif
