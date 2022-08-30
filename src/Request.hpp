#ifndef REQUEST_HPP
# define REQUEST_HPP

//# include "webserv.hpp" // for split
# include <vector>
# include <iostream>
# include <string>
# include <map>

class Request {
private:
	std::string method;	// method는 몇개 없으므로 enum, 매크로상수(int) 등으로 해도 좋을 듯 함.
	std::string path;
	std::string version;
	std::map<std::string, std::string> headers;

	std::string content; // post로 들어오는 데이터 등
public:
	Request(std::string& request);
	~Request();
	std::string getMethod();
	std::string getPath();
	std::string getVersion();
	std::string getHeaderByKey(std::string& key);	// map을 수정하지 않고 조회목적으로만 등록하는게 좋을듯함.
	std::string getContent();

};

#endif