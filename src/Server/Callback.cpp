#include "Server.hpp"
#include <sys/_types/_intptr_t.h>

void Server::exit_with_perror(const string& msg)
{
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}

int Server::callback_error(int fd)
{
	if (is_listensocket(fd))
		exit_with_perror("server socket error");
	else if (is_client(fd))
	{
		std::cerr << "Client socket[" << fd << "] got error" << std::endl;
		disconnect_client(fd);
	}
	else if (is_pipe(fd))
	{
		std::cerr << "Pipe socket[" << fd << "] got error" << std::endl;
		disconnect_pipe(fd);
	}
	return 0;
}

int Server::callback_read(int fd, intptr_t datalen)
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
		else if (ret == 0)
			return (0);
		{
			// 이름 validate으로 바꿀 것.
			// 여기서 걸러진 요청은 바로 write할 수 있게 바꿔야 한다.
			execute_client_request(cli->getFd());

			// 검증단계에서 응답코드가 정해진 것들은 바로 응답한다.
			if (cli->getRequest()->getStatusCode() != 0)
			{
				cli->setResponse(new Response(cli->getRequest()->getStatusCode()));
				cli->getResponse()->makeContent("test\n");
				change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				return 0;
			}

			string file_name = cli->getRequest()->getReqFileName(); // 슬래시 붙어있음
			string dir_path = cli->getRequest()->getLocBlock().getRootDir();
			string final_path = "";
			int flag;
			int ret = 0;

			// Response 생성 및 필요한 인자 전달.
			cli->setResponse(new Response(cli->getRequest()->getStatusCode()));

			// 메소드별로 실행한다.
			if (cli->getRequest()->getMethod() == "GET"){
				ret = cli->GET(cli->getRequest(), cli->getResponse(), dir_path + file_name);
			}
			else if (cli->getRequest()->getMethod() == "DELETE") {
				ret = cli->DELETE(cli->getRequest(), cli->getResponse(), dir_path + file_name);
			} else if (cli->getRequest()->getMethod() == "POST") {
				ret = cli->POST(cli->getRequest(), cli->getResponse(), dir_path + file_name);
			} else if (cli->getRequest()->getMethod() == "PUT") {
				ret = cli->POST(cli->getRequest(), cli->getResponse(), dir_path + file_name);
			} else {
				std::cerr << "Undefined Method" << std::endl;
			}

			if (ret > 2)	// if file descriptor is returned..
			{
				std::cout << "File open! registered to kqueue..." << std::endl;
				file_to_client[ret] = cli->getFd();

				struct stat temp;
				fstat(ret, &temp);
				if (temp.st_mode & O_RDONLY)
					change_events(ret, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
				else
					change_events(ret, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

				//change_events(ret, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			}
			else
			{
				change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			}

			// 원래의 cgi 처리 로직.
			// if (cli->is_cgi_request(cli->getRequest()))
			// {
			// 	pipe_to_client[cli->getPipeFd()] = cli->getFd();
			// 	change_events(cli->getPipeFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			// }
			// else
			// 	change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		}
	}
	else if (is_pipe(fd))
	{
		std::cout << "pipe socket event\n";
		cli = clients_info[pipe_to_client[fd]];
		if (cli->read_pipe_result() == 0 )
		{
			// pipe 결과물을 모두 가져오기 전까지 진행하지 않는다.
			return (0);
		}

		disconnect_pipe(cli->getPipeFd());
		change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
	else if (is_file(fd))
	{
		std::cout << "file read event" << std::endl;
		std::cout << "you have [" << datalen << "] byte left to read" << std::endl;
		cli = clients_info[file_to_client[fd]];
		Request* req = cli->getRequest();
		Response* res = cli->getResponse();

		// 읽기가 완성되지 않았다면 로직중지.
		bool is_read_done = res->readFile(fd, datalen);
		if (!is_read_done)
			return 0;

		if (cli->is_cgi_request(req))
		{
			// 읽기 완료한 내용(sample)을 cgi의 파이프 입구에 write해야 한다.
			// cli->getResponse()->getContent()
		}
		else
			change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	}
	return (0);
}

int Server::callback_write(int fd, intptr_t datalen)
{
	Client *cli;

	if (is_file(fd))
	{
		cli = clients_info[file_to_client[fd]];
		if (cli->getRequest()->getMethod() != "POST")
			return 0;

		std::cout << "file write event" << std::endl;
		Request* req = cli->getRequest();
		Response* res = cli->getResponse();

		bool is_write_done = res->writeFile(fd, datalen);
		if (is_write_done)
			change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		return 0;
	}
	// 클라이언트에게만 write합니다.
	else if (!is_client(fd))
		return -1;

	cli = clients_info[fd];
	// CGI process 종료상태 회수
	if (cli->is_cgi_request(cli->getRequest()))
		waitpid(cli->getRequest()->getCgiPid(), NULL, 0);

	// write하기.
	char *res = strdup(cli->getResponse()->getHttpResponse().c_str());

	// DEBUG Response
	std::cout << "====== response start ======\n" << std::endl;
	std::cout << res << std::endl << std::endl;
	std::cout << "====== response end ======" << std::endl;

	// 클라이언트에게 write
	//========================================================
	// 이 부분을 res->writeFile(fd); 을 써서 처리할 수 있을 것 같다.
	int n;
	if ((n = write(fd, res, strlen(res)) == -1))
		std::cerr << "[DEBUG] client write error!" << std::endl;
	else
		std::cout << "[DEBUG] http response complete" << std::endl;
	//========================================================
	free(res);

	// keep-alive 옵션에따라 포트연결 유지여부를 결정한다.
	if (cli->getRequest()->getReqHeaderValue("Connection") != "keep-alive")
	{
		disconnect_client(fd);
		return (0);
	}

	// 사용이 끝난 Res,Req 객체를 삭제한다.
	delete cli->getResponse();
	delete cli->getRequest();
	cli->setRequest(0);
	cli->setResponse(0);
	// kqueue에서 write이벤트 감지를 해제한다.
	change_events(fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, NULL);
	//disconnect_client(fd);
	return (0);
}

void Server::disconnect_pipe(int pipe_fd)
{
	if (pipe_to_client.find(pipe_fd) == pipe_to_client.end())
	{
		std::cout << "There's no pipe that you want to delete!\n";
		return ;
	}
	else if (clients_info.find(pipe_to_client[pipe_fd]) == clients_info.end())
	{
		std::cout << "There's no client that owns that pipe!\n";
		return ;
	}
	// 클라이언트의 파이프fd를 유효하지 않게 바꾼다.
	clients_info[pipe_to_client[pipe_fd]]->setPipeFd(-1);
	pipe_to_client.erase(pipe_fd);
	close(pipe_fd);
}

void Server::disconnect_client(int client_fd)
{
	close(client_fd);
	Client *cli = clients_info[client_fd];
	if (clients_info[client_fd]->getPipeFd() != -1)
		disconnect_pipe(clients_info[client_fd]->getPipeFd());

	delete cli->getResponse();
	delete cli->getRequest();
	cli->setRequest(0);
	cli->setResponse(0);

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

	// kqueue로부터 읽기 이벤트 감지
	change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	return (0);
}




