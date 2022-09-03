#include "webserv.hpp"

// Server side C program to demonstrate Socket programming
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <vector>
#include <iostream>

#include "Request/Request.hpp"

void make_env(char **env) {
	env[0] = strdup("AUTH_TYPE=");
	env[1] = strdup("CONTENT_LENGTH=\0");
	env[2] = strdup("CONTENT_TYPE=\0");
	env[3] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[4] = strdup("PATH_INFO=/cgi/bye");
	env[5] = strdup("PATH_TRANSLATED=index.html");	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑. 요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑.
	env[6] = strdup("QUERY_STRING=");	// 경로 뒤의 요청 URL에 포함된 조회 문자열.
	env[7] = strdup("REMOTE_ADDR=127.0.0.1");	// 요청을 보낸 클라이언트 IP 주소.
	env[8] = strdup("REMOTE_IDENT=");	// Identification. 클라이언트에서 GCI 프로그램을 실행시킨 사용자.
	env[9] = strdup("REMOTE_USER=");	// 사용자가 인증된 경우 이 요청을 작성한 사용자의 로그인을 의미.	null (인증되지 않음)
	env[10] = strdup("REQUEST_METHOD=GETq");	// 요청 HTTP 메소드 이름. (GET, POST, PUT)
	env[11] = strdup("REQUEST_URI=/cgi/bye");	// 현재 페이지 주소에서 도메인을 제외한 값.
	env[12] = strdup("SCRIPT_NAME=cgi");	// HTTP 요청의 첫 번째 라인에 있는 조회 문자열까지의 URL.
	env[13] = strdup("SERVER_NAME=webserv");	// 요청을 수신한 서버의 호스트 이름.
	env[14] = strdup("SERVER_PORT=4242");	// 요청을 수신한 서버의 포트 번호.
	env[15] = strdup("SERVER_PROTOCOL=HTTP/1.1");	// 요청이 사용하는 프로토콜의 이름과 버전. 	protocol/majorVersion.minorVersion 양식
	env[16] = strdup("SERVER_SOFTWARE=");	// 서블릿이 실행 중인 컨테이너의 이름과 버전.
	env[17] = 0;
}

//=========================================
// Make Response data here!
//=========================================
const char* make_response(Client& client, string& response)
{
	pid_t pid;
	int to_child[2];
	int to_parent[2];

	// dummy data
	string protocol = "HTTP/1.0 404 KO\r\n";
	string servName = "Server:simple web server\r\n";
	string cntLen = "Content-length:2048\r\n";
	string cntType = "Content-type:text/html; charset=UTF-8\r\n\r\n";
	string content = "<html><head><title>Default Page</title></head><body><h1>Hello World!</h1></body></html>";

	// cgi meta-variable 생성


	{ // CGI 돌리기.
		//env = (char**)malloc(sizeof(char*) * 18);
		//make_env(env);
		pipe(to_child);
		pipe(to_parent);

		char **env;
		env = (char**)malloc(sizeof(char*) * 18);
		make_env(env);
		// 파이프 fd를 nonblock하면 어떻게 되는 거지?
		// fcntl(m_pipe[1], F_SETFL, O_NONBLOCK);
		// fcntl(m_pipe[0], F_SETFL, O_NONBLOCK);

		// 자식(CGI)가 가져갈 표준입력 준비.
		char buf[BUF_SIZE + 1];
		memset(buf, 0, sizeof(buf));
		const char *body = strdup(client.getRequest().getReqBody().c_str()); // 왜 warning?
		// -> 이거 스트링이 저장되지 않아서 char로 변형하면 원본이 없으니까 문제생김
		memcpy(buf, body, strlen(body));
		buf[strlen(body)] = 26;	// EOF값을 준다.
		printf("Buf: [%s]\n", buf);
		write(to_child[1], buf, sizeof(buf));


		pid = fork();
		if (pid == 0) {	// child
			printf("[child]Before Execve\n");
			dup2(to_child[0], 0);	// 부모->자식파이프의 읽기fd == 자식의 표준입력
			dup2(to_parent[1], 1);	// 자식->부모파이프 쓰기fd == 자식의 표준출력
			close(to_child[1]);
			close(to_child[0]);
			close(to_parent[1]);
			close(to_parent[0]);
			//if (execve("./cgi_tester", 0, env) == -1) {
			if (execve("./src/cgi_tester", 0, env) == -1) {
				std::cerr << "[child]cgi error\n";
				return NULL;
			}
		} else { // parent
			close(to_child[0]);
			close(to_child[1]);
			int status;

			// 실제론, 기다리지 않도록 구현할 것이다.
			waitpid(-1, &status, 0);
			// read하기.
			close(to_parent[1]);	// 이게 없으면 EOF검출이 안된다?
			string result = "";
			while((status = read(to_parent[0], buf, BUF_SIZE)) > 0 && strlen(buf) != 0) {
				buf[status] = '\0';
				// 왜 ret, len이 다른거지...?
				printf("\n%s[ret:%d, len:%lu Loop...]\n", buf, status, strlen(buf));
				string temp(buf);
				result += temp;
			}
			cout << "result: " << result << endl;
		}
	}
	// result로 response를 완성하시오.

	//response = protocol+servName+cntLen+cntType+content;
	return response.c_str();
}

void exit_with_perror(const string& msg)
{
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

void disconnect_client(int client_fd, map<int, string>& clients)
{
	cout << "client disconnected: " << client_fd << endl;
	close(client_fd);
	clients.erase(client_fd);
}

void change_events(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

void SendErrorMSG(int sock)
{
	char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
	char servName[] = "Server:simple web server\r\n";
	char cntLen[] = "Content-length:2048\r\n";
	char cntType[] = "Content-type:text/html\r\n\r\n";
	char content[] = "<html><body><h1>You got Error!</h1></body></html>";

	send(sock, protocol, strlen(protocol), 0);
	send(sock, servName, strlen(servName), 0);
	send(sock, cntLen, strlen(cntLen), 0);
	send(sock, cntType, strlen(cntType), 0);
	send(sock, content, strlen(content), 0);
	close(sock);
}

int main(int argc, char **argv)
{
	int			client_socket;
	sockaddr_in	client_addr;
	socklen_t	client_len;



	// for tcp/ip
	int server_fd, new_socket; long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	// for kqueue
	struct	kevent event;	 /* 감시대상 이벤트 */
	struct	kevent tevent;	 /* Event triggered */
	int kq, ret;
	// 필요?
	int flags;
	struct timespec timeout = { 0, 0 };

	if (argc != 2)
		err(EXIT_FAILURE, "Usage: %s <Port>\n", argv[0]);

	// 서버소켓 셋팅.
	{
		// 소켓 생성
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("In socket");
			exit(EXIT_FAILURE);
		}

		// 주소 구조체에 서버의 IP,PORT 저장
		memset(address.sin_zero, 0, sizeof(address.sin_zero));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons( atoi(argv[1]) );

		// Bind
		if (bind(server_fd, (struct sockaddr *)&address, (socklen_t)sizeof(address)) < 0)
		{
			perror("In bind");
			exit(EXIT_FAILURE);
		}
		// Listen
		if (listen(server_fd, 10) < 0)
		{
			perror("In listen");
			exit(EXIT_FAILURE);
		}
	}

	// kqueue 셋팅
	{
		/* Create kqueue. */
		kq = kqueue();
		if (kq	== -1)
			err(EXIT_FAILURE, "kqueue() failed");

		/* Initialize kevent structure. */
		// kev, ident, filter, flags, fflags, data, udata
		EV_SET(&event, server_fd, EVFILT_READ, EV_ADD | EV_CLEAR, flags,
			0,	NULL);

		/* Attach event to the	kqueue.	*/
		// kq, changelist, nchanges, eventlist, nevents, timeout
		ret = kevent(kq, &event, 1, NULL, 0, NULL);
		printf("kevent ret : %d\n", ret);
		if (ret == -1)
			err(EXIT_FAILURE, "kevent register");
		if (event.flags & EV_ERROR)
			errx(EXIT_FAILURE,	"Event error: %s", strerror(event.data));
	}

	map<int, string> clients; // map for client socket:data
	vector<struct kevent> change_list; // kevent vector for changelist
	struct kevent event_list[8]; // kevent array for eventlist

	/* add event for server socket */
	change_events(change_list, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	cout << "echo server started" << endl;

	// 통신.
	int new_events;
	struct kevent* curr_event;
	for (;;)
	{
		// kevent 호출
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
		// change_list 비우기
		change_list.clear();

		// 이벤트리스트순회
		//cout << "\nEvent list travel, nEvent : << " << new_events << "\n";
		for(int i=0;i < new_events ;i++)
		{
			curr_event = &event_list[i];
			// 해당 이벤트가 에러이벤트라면
			if (curr_event->flags & EV_ERROR)
			{
				// 서버소켓의 이벤트라면 exit
				if (curr_event->ident == server_fd)
					exit_with_perror("server socket error");
				// 클라이언트소켓 이벤트라면 disconnect
				else
				{
					cerr << "client socket[" << curr_event->ident << "] got error" << endl;
					disconnect_client(curr_event->ident, clients);
				}
			}
			// 해당 이벤트가 읽기이벤트라면
			else if (curr_event->filter == EVFILT_READ)
			{
				// 서버소켓의 이벤트라면 accept
				if (curr_event->ident == server_fd)
				{
					// int			client_socket;
					// sockaddr_in	client_addr;
					// socklen_t	client_len;
					// 클소켓을 change_list에 읽쓰이벤트로 등록
					if ((client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_len)) == -1)
						exit_with_perror("accept error");
					cout << "client socket[" << client_socket << "] just connected" << endl;
					change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					// 클라이언트의 요청저장을 위해 map에 공간할당.
					clients[client_socket] = "";
				}
				// 클소켓의 이벤트라면
				else if (clients.find(curr_event->ident) != clients.end())
				{
					char buf[1024];
					int n = read(curr_event->ident, buf, sizeof(buf));
					// read 결과가 0미만일시 disconnect
					if (n <= 0)
					{
						if (n < 0)
							cerr << "client read error!" << endl;
						disconnect_client(curr_event->ident, clients);
					}
					// 그외 데이터 저장
					else
					{
						buf[n] = '\0';
						clients[curr_event->ident] += buf;
						cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << endl;
					}
				}
			}
			// 해당 이벤트가 쓰기이벤트라면
			else if (curr_event->filter == EVFILT_WRITE)
			{
				map<int, string>::iterator it = clients.find(curr_event->ident);
				if (it != clients.end())
				{
					if (clients[curr_event->ident] != "") // 보낼문자열이 있을때만. -
					{
						string response;

						Client *c_ptr = new Client(client_socket, client_addr, client_len, *(new Request(clients[curr_event->ident])));
						cout << "fd : " << c_ptr->getFd() << endl;
						cout << "path : " << c_ptr->getRequest().getReqTarget() << endl;
						cout << "version : " << c_ptr->getRequest().getHttpVersion() << endl;
						string str = "Host";
						cout << "host : " << c_ptr->getRequest().getReqHeaderValue(str) << endl;
						const char *res = make_response(*c_ptr, response);

						// 클라이언트에게 write
						int n;
						if ((n = write(curr_event->ident, res, strlen(res)) == -1))
						{
							cerr << "client write error!" << endl;
							disconnect_client(curr_event->ident, clients);
						}
						else
						{
							clients[curr_event->ident].clear();	// echo 이후 보낼 문자열을 지운다.
							cout << "http response complete" << endl;
						}
						//SendErrorMSG(curr_event->ident);
						disconnect_client(curr_event->ident, clients);
					}
				}
			}
		}
	}
}
