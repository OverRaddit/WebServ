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
		else if (ret == 0)
			return (0);
		{
			// 이름 validate으로 바꿀 것.
			execute_client_request(cli->getFd());
			string file_name = cli->getRequest()->getReqFileName(); // 슬래시 붙어있음
			//string dir_path = cli->getRequest()->getSudoDir();
			string dir_path = cli->getRequest()->getLocBlock().getRootDir();
			string final_path = "";
			int flag;

			switch (cli->getRequest()->getReqType())
			{
			case CGI_REQUEST:
				std::cout << "Req type: CGI" << std::endl;
				if ((ret = cli->cgi_init()) < 0)
					return -1;
				pipe_to_client[cli->getPipeFd()] = cli->getFd();
				change_events(cli->getPipeFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
				break;
			case UPLOAD_REQUEST:
				std::cout << "Req type: UPLOAD" << std::endl;
				cli->setResponse(new Response(cli->getRequest()->getStatusCode()));
				cli->getResponse()->makeContent("Upload Request");
				//cli->getResponse()->uploadResponse(cli->getRequest()->getReqHeaderValue("Content-Type"), cli->getRequest()->getReqBody());
				change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				break;
			case OTHER_REQUEST:
				cli->setResponse(new Response(cli->getRequest()->getStatusCode()));
				// index 고려할것..
				if (file_name == "")
					cli->getResponse()->defaultResponse();
				else {
					cout << "CHANGE REQ URL : " << dir_path + file_name << endl;
					flag = cli->getResponse()->getRequestFile(file_name, dir_path);
					if (flag == NO_FILE)
					{
						//cli->getResponse()->makeContent("No such file"); // 404
						cli->getResponse()->errorResponse(404);
					}
					else if (flag == VALID_REQ_DIR)
					{
						if (file_name.back() == '/')
							final_path = dir_path + file_name;
						else
							final_path = dir_path + file_name + "/";
						cout << "VALID_REQ_DIR1 : " << final_path + cli->getRequest()->getLocBlock().getIndexFile() << endl;
						if (cli->getResponse()->getRequestFile(cli->getRequest()->getLocBlock().getIndexFile(), final_path) == NO_FILE)
						{
							cli->getResponse()->makeContent("No such file"); // 404
							cli->getResponse()->setStatusCode(404);
						}
						else
						{
							cout << "VALID_REQ_DIR2 : " << dir_path + "/" + cli->getRequest()->getLocBlock().getIndexFile() << endl;
							cli->getResponse()->fileResponse(dir_path + "/" + cli->getRequest()->getLocBlock().getIndexFile());
							cli->getResponse()->setStatusCode(200);
						}
					}
					else if (flag == VALID_REQ_FILE)
					{
						cout << "VALID_REQ_FILE : " << dir_path + file_name << endl;
						cli->getResponse()->fileResponse(dir_path + file_name);
						cli->getResponse()->setStatusCode(200);
					}
				}
				change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				break;
			case DELETE_REQUEST:
				std::cout << "Req type: DELETE" << std::endl;
				cli->setResponse(new Response(cli->getRequest()->getStatusCode()));
				cli->getResponse()->makeContent("DELETE REQUEST");
				// cli->getResponse()->deleteResponse(dir_path + cli->getRequest()->getDelFileName());
				change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				break;
			case AUTOINDEX_REQUEST:
				std::cout << "Req type: AUTOINDEX" << std::endl;
				cli->setResponse(new Response(cli->getRequest()->getStatusCode()));
				cli->getResponse()->autoIndexResponse(dir_path.c_str());
				change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				break;
			case DOWNLOAD_REQUEST:
				std::cout << "Req type: DOWNLOAD" << std::endl;
				cli->setResponse(new Response(cli->getRequest()->getStatusCode()));
				cli->getResponse()->downloadResponse(dir_path + "a.txt");
				change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				break;
			default:
				std::cout << "Req type: " << cli->getRequest()->getReqType() << std::endl;
				break;
			}
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
	return (0);
}

int Server::callback_write(int fd)
{
	Client *cli;

	// 클라이언트에게만 write합니다.
	if (!is_client(fd))
		return -1;

	cli = clients_info[fd];
	// CGI process 종료상태 회수
	if (cli->getRequest()->getReqType() == CGI_REQUEST)
		waitpid(cli->getRequest()->getCgiPid(), NULL, 0);

	// write하기.
	// 가끔 이미 처리한 요청을 또 write해서 req,res가 없다.
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




