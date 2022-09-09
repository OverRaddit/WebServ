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
	std::string	raw_request;	// HTTP 요청 원본

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
	std::string	getRawRequest() const;

	void		setPipeFd(int pipe_fd);
	void		setRequest(Request *_req);
	void		setRawRequest(std::string str);
	void		appendRawRequest(std::string _raw_request);

	int			read_client_request();
	int			read_pipe_result();
	void		make_env(char **env);
	int			cgi_init();
};

#endif