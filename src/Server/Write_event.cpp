#include "Server.hpp"

int Server::client_write(int fd, intptr_t datalen)
{
	Client *cli = clients_info[fd];
	// CGI process 종료상태 회수
	if (cli->is_cgi_request(cli->getRequest()))
		waitpid(cli->getRequest()->getCgiPid(), NULL, 0);

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
	free(res);
	//========================================================

	// keep-alive 옵션에따라 포트연결 유지여부를 결정한다.
	if (cli->getRequest()->getReqHeaderValue("Connection") != "keep-alive") // curl
	{
		std::cout << "Keep-alive header off!" << std::endl;
		disconnect_client(fd);
		return (0);
	}

	std::cout << "Keep-alive header on!" << std::endl;
	// 사용이 끝난 Res,Req 객체를 삭제한다.
	delete cli->getResponse();
	delete cli->getRequest();
	cli->setRequest(0);
	cli->setResponse(0);

	// kqueue에서 write이벤트 감지를 해제한다.
	change_events(fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, NULL);
	return (0);
}

int Server::pipe_write(int fd, intptr_t datalen)
{
	Client *cli = clients_info[pipe_to_client[fd]];
	Request* req = cli->getRequest();
	Response* res = cli->getResponse();
	Cgi* cgi = cli->getCgi();

	if (!res->writeFile(fd, res->getContent().length()))
		return 0;

	// write done in pipe input.
	pipe_to_client.erase(fd);	// 파이프 입구를 파이프목록에서 삭제
	close(fd);					// 파이프 입구를 close
	change_events(cgi->getToParent()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	//=====================
	return 0;
}

int Server::file_write(int fd, intptr_t datalen)
{
	//std::cout << "file write event" << std::endl;
	Client *cli = clients_info[file_to_client[fd]];
	Request* req = cli->getRequest();
	Response* res = cli->getResponse();

	// 애초에 파일이벤트를 등록할때 READ,WRITE따로 등록하는데 이부분이 필요할까? =================
	// file이 Read-only라면, write할 수 없음.
	if (cli->getResponse()->getFdMode(fd) == O_RDONLY)
		return 0;
	//============================================================================
	// datalen == 1 ?? why?
	// 한번에 1씩만 쓸수있다...?
	bool is_write_done = res->writeFile(fd, datalen);
	if (is_write_done)
		change_events(cli->getFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	return 0;
}