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
	Request		*req;
public:
	Client();
	Client(int fd, sockaddr_in addr, socklen_t len);
	~Client();
	Client(const Client& a);
	Client& operator=(const Client& a);

	int			getFd() const;
	int			getPipeFd() const;
	sockaddr_in	getAddr() const;
	socklen_t	getLen() const;
	Request		*getRequest();

	void		setPipeFd(int pipe_fd);
	void		setRequest(Request *_req);
	std::vector<std::string>**	getCGIEnv();
};

#endif