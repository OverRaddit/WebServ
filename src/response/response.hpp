#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>

using namespace std;

class Response {
private:
	static map<int, string>	m_status_description;
	int						m_status_code;
	map<string, string>		m_headers;
	string					m_content;
	string					m_cgiResult;

	string		makeHeaders();

public:
	// 왜 private??
	Response ();
	~Response ();

	static void	ResponseInit(); // Response 클래스를 초기화하는 한번만 실행 가능함수
	void		makeContent();

	int					getStatusCode();
	map<int ,string>	getStatusDesc();
	map<string,string>	getHeaders();
	string				getContent();

	void		setStatusCode(int code);
	void		setStatusDesc(int code, string desc);
	void		setHeaders(string key, string value);
	void		setCgiResult(string ret);
	void		setContent(string content);

	string		getHttpResponse();
};

#endif
