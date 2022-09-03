#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
//#include "../Request/Request.hpp"

//#include "./cgi.hpp"

using namespace std;

class Response {
private:
	int						m_status_code;
	static map<int ,string>	m_status_description;
	map<string, string>		m_headers;
	string					m_content;
	string					m_cgiResult;
	//Request					*request;

	static bool				initFlag;

	void		makeStatus200(int code);
	void		makeStatus300(int code);
	void		makeStatus400(int code);
	void		makeStatus500(int code);

	string		makeHeaders();
	string		makeContent();

public:
	Response ();
	~Response ();

	static void			ResponseInit(); // Response 클래스를 초기화하는 한번만 실행 가능함수

	int					getStatusCode();
	map<int ,string>	getStatusDesc();
	map<string,string>	getHeaders();
	string				getContent();

	void		setStatusCode(int code);
	void		setStatusDesc(int code, string desc);
	void		setHeaders(string key, string value);
	void		setContent(string content);

	string		getHttpResponse();
};

#endif
