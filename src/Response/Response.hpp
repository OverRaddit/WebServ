#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <unistd.h>
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <string.h>
# include <fcntl.h>

# include <string>
# include <map>
# include <vector>
# include <fstream>
# include <cctype>
# include <iostream>
# include <utility>

# include "../Config/Config.hpp"

# define NO_FILE -1
# define VALID_REQ_FILE 1
# define VALID_REQ_DIR 0

# define BUFF_SIZE 1024

using namespace std;

class Response {
private:
	static map<int, string>	m_status_description;
	static bool				is_init;
	int						m_status_code;
	map<string, string>		m_headers;
	string					m_content;
	string					m_cgiResult;
	//string					m_rootPath;
	//string					m_indexFile;
	//string					m_ErrorFile;
	LocationBlock			m_location;

	string		makeHeaders();
	string		parseHeader(string& content_type);
	string		getFileContent(string& content_type, string last_boundary);

	int			saveFile(int fd, string content_type, string content_body);  // m_content로 받을 데이터를 파싱해서 파일로 저장하는 함수
	void		serveFile(int fd);
	int			deleteFile(string file_path);
	void		putFile(vector<int> fd, string content_body);

	int			getFileList(vector<string>& li, const char *dir_path);
	int			makeAutoIndex(const char *dir_path);

	void		makeContentError(int status, int fd);
	void		makeContentFile(int fd);

public:
	// 왜 private??
	Response ();
	Response (int status);
	~Response ();

	// 임시로 public 이동
	//int			serveFile(string file_path);

	static void	ResponseInit(); // Response 클래스를 초기화하는 한번만 실행 가능함수
	void		makeContent(string content);

	int					getStatusCode();
	map<int ,string>	getStatusDesc();
	map<string,string>	getHeaders();
	string				getContent();
	int					getRequestFile(string request_file, string dir_path);

	void		setStatusCode(int code);
	void		setStatusDesc(int code, string desc);
	void		setHeaders(string key, string value);
	void		setCgiResult(string ret);
	void		setContent(string content);

	//void		setRootPath(string path);
	//void		setIndexFile(string path);
	//void		setErrorFile(string path);
	void		setLocationBlock(LocationBlock loc);

	void		cgiResponse(string cgi_result);  // cgi 결과를 요청하는 경우의 응답처리
	void		uploadResponse(vector<int> fd, string content_type, string content_body);  // 파일 업로드 경우의 응답처리
	void		downloadResponse(string file_path);  // 파일 다운로드 응답처리
	void		deleteResponse(string file_path);  // 파일 삭제 응답처리
	void		autoIndexResponse(const char *dir_path);
	void		errorResponse(int fd, int status);
	void		defaultResponse(int fd);
	void		fileResponse(int fd);

	vector<pair<string, string> >	saveFileName(string content_type, string content_body);

	int			openFile(string path);
	vector<int>	openFiles(vector<pair<string, string> > in);
	string		readFile(int fd);
	ssize_t		writeFile(int fd, string content);

	string		getHttpResponse();
};

// encode와 decode관련 비멤버 함수
char hex2int(char input);
char int2hex(char input);
string URLEncoding(const char *pIn);
string URLDecoding(const char *pIn);
bool is_directory(const char *suffix_url);
#endif
