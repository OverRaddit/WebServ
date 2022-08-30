#include "Request.hpp"

void split(const std::string& str, const std::string& delimiters , std::vector<std::string>& tokens)
{
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

Request::Request(std::string& request)
{
	std::vector<std::string> tokens;
	split(request, "\r\n", tokens);

	std::vector<std::string> FirstLine;
	split(tokens[0], " ", FirstLine);
	method = FirstLine[0];
	path = FirstLine[1];
	version = FirstLine[2];

	int i = 1;
	// Set Headers
	for(;i<tokens.size() && tokens[i].size()!=0 ;i++)
	{
		std::vector<std::string> temp;
		split(tokens[i], ": ", temp);
		//cout << "key: " << temp[0] << ", value: " << temp[1] << endl;;
		headers[ temp[0] ] = temp[1];
	}

	// Set Content
	for(;i<tokens.size();i++)
	{
		content += tokens[i] + '\n';
	}
	printf("Client's content:");
}
Request::~Request(){}

std::string Request::getMethod(){ return method; }
std::string Request::getPath(){ return path; }
std::string Request::getVersion(){ return version; }
std::string Request::getHeaderByKey(std::string& key){ return headers[key]; }
std::string Request::getContent(){ return content; }