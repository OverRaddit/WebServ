#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../Request.hpp"
# include <sys/socket.h>
# include <netinet/in.h>

class Client
{
private:
	int			fd;
	int			pipe_fd;
	sockaddr_in	addr;
	socklen_t	len;
	Request&	req;
public:
	Client(int fd, sockaddr_in addr, socklen_t len, Request& req);
	~Client();

	int			getFd();
	int			getPipeFd();
	sockaddr_in	getAddr();
	socklen_t	getLen();
	Request&	getRequest();

	void		setPipeFd(int pipe_fd);

	std::vector<std::string>**	getCGIEnv();
};

#endif