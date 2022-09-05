#include "Client.hpp"

Client::Client()
	: fd(-1), len(0), req(NULL), pipe_fd(-1)
{
	//addr = -1;
}

Client::Client(int fd, sockaddr_in addr, socklen_t len)
	: fd(fd), addr(addr), len(len), req(NULL), pipe_fd(-1)
{
	std::cout << "Client Constructor!" << std::endl;
}

Client::~Client()
{
	std::cout << "Client Destructor!" << std::endl;
	if (req)
		delete req;
}

Client::Client(const Client& a) : fd(a.fd), pipe_fd(a.pipe_fd), addr(a.addr), len(a.len)
{
	req = new Request(*(a.req));
}

Client& Client::operator=(const Client& a)
{
	fd = a.fd;
	pipe_fd = a.pipe_fd;
	addr = a.addr;
	len = a.len;
	req = new Request(*(a.req));

	return *this;
}

int	Client::getFd() const{ return fd; }
int	Client::getPipeFd() const{ return pipe_fd; }
sockaddr_in	Client::getAddr() const{ return addr; }
socklen_t	Client::getLen() const{ return len; }
Request*	Client::getRequest(){ return req; }

void		Client::setPipeFd(int _pipe_fd){ pipe_fd = _pipe_fd; }
void		Client::setRequest(Request *_req){ req = _req; }


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

// std::ostream& operator<<(std::ostream& out, const Client& cli)
// {
// 	out << "fd : " << cli.getFd() << std::endl;
// //	out << "path : " << cli.getRequest().getReqTarget() << std::endl;
// 	out << "version : " << cli.getRequest()->getHttpVersion() << std::endl;
// 	std::string str = "Host";
// 	out << "host : " << cli.getRequest()->getReqHeaderValue(str) << std::endl;

// 	return out;
// }