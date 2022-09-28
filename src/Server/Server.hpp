#ifndef Server_HPP
# define Server_HPP

# include "../Client/Client.hpp"
# include "../Config/Config.hpp"
# include <iostream>
# include <utility>
# include <string>
# include <map>
# include <vector>

# include <sys/socket.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/event.h>
# include <err.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

class Server {

private:
	std::map<int, Client*>	clients_info;
	std::map<int, int>		pipe_to_client;

	Config					*config;

	// for socket
	int server_fd;
	struct sockaddr_in address;
	int addrlen;

	// for kqueue
	int kq_fd;
	std::vector<struct kevent> change_list;		// kevent vector for changelist
	struct kevent event_list[8];			// kevent array for eventlist

public:
//=============================================================================
//	Orthodox Canonical Form
//=============================================================================
	Server();							// 기본생성자
	Server(string config_filepath);		// 생성자
	Server(const Server& a);			// 복사생성자
	Server& operator=(const Server& a);	// 대입연산자
	~Server();							// 소멸자

//=============================================================================
//	Method
//=============================================================================
	int init_socket(int port);
	int init_multiplexing();
	int run();

	int callback_error(int fd);
	int callback_read(int fd);
	int callback_write(int fd);

	void exit_with_perror(const string& msg);
	void disconnect_client(int client_fd);
	void change_events(uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata);


	int connect_new_client();
	int read_client_request();
	int read_pipe_result();

//=============================================================================
//	Getter & Setter
//=============================================================================
	void	setReqBody(string req_body);
	void	setReqHeader(string key, string value);

	string	getReqHeaderValue(string key);
	string	getMethod(void) const;
	string	getReqTarget(void) const;
	string	getHttpVersion(void) const;
	string	getReqBody(void) const;

};

#endif
