#include "Server.hpp"

int Server::callback_error(int fd)
{
	// 서버소켓의 이벤트라면 exit
	if (fd == server_fd)
		exit_with_perror("server socket error");
	// 클라이언트소켓 이벤트라면 disconnect
	else
	{
		std::cerr << "client socket[" << fd << "] got error" << std::endl;
		disconnect_client(fd, clients);
	}
}

int Server::callback_read(int fd)
{
	Client *cli;


	if (fd == server_fd) // 서버소켓
		connect_new_client();	// 서버의 동작
	else if (clients_info.find(fd) != clients_info.end()) // 클라이언트
	{
		int ret;
		cli = clients_info[fd];
		if ((ret = cli->read_client_request()) < 0)
			disconnect_client(client_fd);
		pipe_to_client[ret] = cli.getFd();
		change_events(ret, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
	else if (pipe_to_client.find(fd) != pipe_to_client.end()) // 파이프
	{
		cli = clients_info[fd];
		cli->read_pipe_result();
		if (cli->getPipeFd() != -1)
			pipe_to_client.erase(fd);
		change_events(change_list, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
}

int Server::callback_write(int fd)
{
	Client *cli;

	// 클라이언트에게만 write합니다.
	if (clients_info.find(fd) == clients_info.end())
		return ;

	// write하기.
	cli = clients_info[fd];
	const char *res = strdup(cli->getRawRequest().c_str());
	std::cout << "response: " << res << std::endl;
	// 클라이언트에게 write
	int n;
	if ((n = write(fd, res, strlen(res)) == -1))
		std::cerr << "client write error!" << std::endl;
	else
		std::cout << "http response complete" << std::endl;
	disconnect_client(fd);
}

void Server::disconnect_client(int client_fd)
{
	close(client_fd);
	clients_info.erase(client_fd);
	delete clients_info[client_fd];
	std::cout << "[DEBUG]client disconnected: " << client_fd << std::endl;
}

void Server::change_events(uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

int Server::connect_new_client()
{
	int			client_socket;
	sockaddr_in	client_addr;
	socklen_t	client_len;

	// 클소켓을 change_list에 읽쓰이벤트로 등록
	if ((client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_len)) == -1)
		exit_with_perror("accept error");
	std::cout << "[DEBUG]client socket[" << client_socket << "] just connected" << std::endl;

	// client 정보 등록
	clients_info[client_socket] = new Client(client_socket, client_addr, client_len);
	std::cout << "[DEBUG]Fd is " << clients_info[client_socket]->getFd() << std::endl;

	// kqueue로부터 읽기 이벤트 감지
	change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}




