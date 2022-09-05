#include "Client.hpp"

Client::Client(int fd, sockaddr_in addr, socklen_t len, Request& req)
	: fd(fd), addr(addr), len(len), req(req)
{
	std::cout << "Client Constructor!" << std::endl;
}

Client::~Client()
{
	std::cout << "Client Destructor!" << std::endl;
}

int	Client::getFd(){ return fd; }
int	Client::getPipeFd(){ return pipe_fd; }
sockaddr_in	Client::getAddr(){ return addr; }
socklen_t	Client::getLen(){ return len; }
Request&	Client::getRequest(){ return req; }

void		Client::setPipeFd(int _pipe_fd){ pipe_fd = _pipe_fd; }


// std::vector<std::string>* Client::getCGIEnv()
// {
// 	std::vector<std::string> *env = new std::vector<std::string>;

// 	env.push_back("AUTH_TYPE=");						// ??
// 	env.push_back("CONTENT_LENGTH=");
// 	env.push_back("CONTENT_TYPE=");
// 	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
// 	env.push_back("PATH_INFO=" + getRequest().getPath());
// 	env.push_back("PATH_TRANSLATED=index.html");			// ??
// 	env.push_back("QUERY_STRING=");
// 	env.push_back("REMOTE_ADDR=127.0.0.1");
// 	env.push_back("REMOTE_IDENT="); //+ std::string(getFd()));
// 	env.push_back("REMOTE_USER=");
// 	env.push_back("REQUEST_METHOD=" + getRequest().getMethod());
// 	env.push_back("REQUEST_URI=" + getRequest().getPath());
// 	env.push_back("SCRIPT_NAME=cgi");
// 	env.push_back("SERVER_NAME=webserv");
// 	env.push_back("SERVER_PORT=4242");
// 	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
// 	env.push_back("SERVER_SOFTWARE=");
// 	env.push_back(NULL);

// 	return &env;
// }