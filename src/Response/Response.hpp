#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include <vector>
# include <fstream>
# include <unistd.h>
# include <cctype>
# include <dirent.h>
# include <sys/types.h>

using namespace std;

class Response {
private:
	static map<int, string>	m_status_description;
	static bool				is_init;
	int						m_status_code;
	map<string, string>		m_headers;
	string					m_content;
	string					m_cgiResult;
	string					m_rootPath;
	string					m_indexFile;
	string					m_ErrorFile;

	string		makeHeaders();
	string		parseHeader(string& content_type);
	string		getFileContent(string& content_type, string last_boundary);

	int			saveFile(string content_type, string content_body);  // m_content로 받을 데이터를 파싱해서 파일로 저장하는 함수
	int			serveFile(string file_path);
	int			deleteFile(string file_path);

	int			getFileList(vector<string>& li, const char *dir_path);
	int			makeAutoIndex(const char *dir_path);

	int			makeContentError(string error_path);
	int			makeContentIndex(string root_path);

public:
	// 왜 private??
	Response ();
	Response (int status);
	~Response ();

	static void	ResponseInit(); // Response 클래스를 초기화하는 한번만 실행 가능함수
	void		makeContent(string content);

	int					getStatusCode();
	map<int ,string>	getStatusDesc();
	map<string,string>	getHeaders();
	string				getContent();

	void		setStatusCode(int code);
	void		setStatusDesc(int code, string desc);
	void		setHeaders(string key, string value);
	void		setCgiResult(string ret);
	void		setContent(string content);

	void		setRootPath(string path);
	void		setIndexFile(string path);
	void		setErrorFile(string path);

	void		cgiResponse(string cgi_result);  // cgi 결과를 요청하는 경우의 응답처리
	void		uploadResponse(string content_type, string content_body);  // 파일 업로드 경우의 응답처리
	void		downloadResponse(string file_path);  // 파일 다운로드 응답처리
	void		deleteResponse(string file_path);  // 파일 삭제 응답처리
	void		autoIndexResponse(const char *dir_path);
	void		defaultResponse();

	string		getHttpResponse();
};

// encode와 decode관련 비멤버 함수
char hex2int(char input);
char int2hex(char input);
string URLEncoding(const char *pIn);
string URLDecoding(const char *pIn);

#endif
