#include "Server.hpp"

void Server::exit_with_perror(const string& msg)
{
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}

int Server::callback_error(int fd)
{
	// 서버소켓의 이벤트라면 exit
	if (is_listensocket(fd))
		exit_with_perror("server socket error");
	// 클라이언트소켓 이벤트라면 disconnect
	else if (clients_info.find(fd) != clients_info.end())
	{
		std::cerr << "Client socket[" << fd << "] got error" << std::endl;
		disconnect_client(fd);
	}
	else if (pipe_to_client.find(fd) != pipe_to_client.end())
	{
		std::cerr << "Pipe socket[" << fd << "] got error" << std::endl;
		disconnect_client(fd);
	}
	return 0;
}

int Server::callback_read(int fd)
{
	Client *cli;

	if (is_listensocket(fd))
		connect_new_client(fd);
	else if (is_client(fd))
	{
		int ret;
		cli = clients_info[fd];
		ret = cli->read_client_request();
		if (ret < 0)
		{
			disconnect_client(fd);
			return (-1);
		}

		execute_client_request(cli->getFd());
		// if ( cli->request 요청 == CGI 요청)
		if ( false )
		{
			pipe_to_client[ret] = cli->getFd();
			change_events(ret, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		}
		else
		{
			cli->setResponse(new Response(cli->getRequest()->getStatusCode()));
			change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		}
	}
	else if (is_pipe(fd))
	{
		cli = clients_info[pipe_to_client[fd]];
		cli->read_pipe_result();
		if (cli->getPipeFd() != -1)
			pipe_to_client.erase(fd);
		change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
	return (0);
}

int Server::callback_write(int fd)
{
	Client *cli;

	// 클라이언트에게만 write합니다.
	if (!is_client(fd))
		return -1;

	// write하기.
	cli = clients_info[fd];
	char *res = strdup(cli->getResponse()->getHttpResponse().c_str());

	// DEBUG Response
	std::cout << "====== response start ======\n" << std::endl;
	std::cout << res << std::endl << std::endl;
	std::cout << "====== response end ======" << std::endl;

	// 클라이언트에게 write
	int n;
	if ((n = write(fd, res, strlen(res)) == -1))
		std::cerr << "[DEBUG] client write error!" << std::endl;
	else
		std::cout << "[DEBUG] http response complete" << std::endl;
	free(res);
	// 사용이 끝난 Res,Req 객체를 삭제한다.
	delete cli->getResponse();
	delete cli->getRequest();
	cli->setRequest(0);
	cli->setResponse(0);
	// kqueue에서 write이벤트 감지를 해제한다.
	change_events(fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, NULL);
	//disconnect_client(fd);
}

void Server::disconnect_client(int client_fd)
{
	close(client_fd);
	if (clients_info[client_fd]->getPipeFd() != -1)
	{
		pipe_to_client.erase(clients_info[client_fd]->getPipeFd());
		close(clients_info[client_fd]->getPipeFd());
	}
	delete clients_info[client_fd];
	clients_info.erase(client_fd);
	std::cout << "[DEBUG]client disconnected: " << client_fd << std::endl;
}

void Server::change_events(uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

int Server::connect_new_client(int fd)
{
	int			client_socket;
	sockaddr_in	client_addr;
	socklen_t	client_len;

	// 클소켓을 change_list에 읽쓰이벤트로 등록
	if ((client_socket = accept(fd, (sockaddr*)&client_addr, &client_len)) == -1)
		exit_with_perror("accept error");
	std::cout << "[DEBUG]client socket[" << client_socket << "] just connected" << std::endl;

	// client 정보 등록
	clients_info[client_socket] = new Client(client_socket, client_addr, client_len);
	//std::cout << "[DEBUG]Fd is " << clients_info[client_socket]->getFd() << std::endl;

	// kqueue로부터 읽기 이벤트 감지
	change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}




