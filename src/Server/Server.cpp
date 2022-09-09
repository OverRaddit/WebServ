/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gshim <gshim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 19:50:39 by gshim             #+#    #+#             */
/*   Updated: 2022/09/07 21:06:20 by gshim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//=============================================================================
//	Orthodox Canonical Form
//=============================================================================
	Server::Server()
	{
		// read config file.

		// init Server config data
	}
	Server::Server(const Server& a){ *this = a }
	Server& Server::operator=(const Server& a){}
	Server::~Server(){}

//=============================================================================
//	Method
//=============================================================================
	Server::init()
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

	// server_fd
	Server::init_multiplexing()
	{
		struct	kevent event;	 /* 감시대상 이벤트 */
		struct	kevent tevent;	 /* Event triggered */
		int flag;

		kq = kqueue();
		if (kq	== -1)
			err(EXIT_FAILURE, "kqueue() failed");

		// flag에 0넣으면?
		EV_SET(&event, server_fd, EVFILT_READ, EV_ADD | EV_CLEAR, flags, 0,	NULL);

		ret = kevent(kq, &event, 1, NULL, 0, NULL);
		if (ret == -1)
			err(EXIT_FAILURE, "kevent register");
		if (event.flags & EV_ERROR)
			errx(EXIT_FAILURE,	"Event error: %s", strerror(event.data));
	}

	Server::run()
	{
		change_events(server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		std::cout << "WebServer started" << std::endl;

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
						int			client_socket;
						sockaddr_in	client_addr;
						socklen_t	client_len;
						// 클소켓을 change_list에 읽쓰이벤트로 등록
						if ((client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_len)) == -1)
							exit_with_perror("accept error");
						cout << "client socket[" << client_socket << "] just connected" << endl;
						change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
						//change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
						// 클라이언트의 요청저장을 위해 map에 공간할당.
						clients[client_socket] = "";
						Client cli(client_socket, client_addr, client_len);
						clients2[client_socket] = &cli;
						std::cout << "[DEBUG]Fd is " << clients2[client_socket]->getFd() << std::endl;
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

							// 자식프로세스 생성
							Client *cli = clients2[curr_event->ident];
							cli->setRequest(new Request(clients[curr_event->ident]));
							std::cout << "[DEBUG]Method is " << cli->getRequest()->getMethod() << std::endl;
							int pipe_fd = make_response(*cli, pipes);
							change_events(change_list, pipe_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
						}
					}
					// 자식프로세스의 파이프라면
					// IF 조건문 필요.
					else if (pipes.find(curr_event->ident) != pipes.end())
					{
						std::cout << "[DEBUG] Pipe fd: " << curr_event->ident << "is ready!" << std::endl;
						int client_fd = pipes[curr_event->ident];
						int pipe_fd = curr_event->ident;
						int ret;
						char buf[BUF_SIZE + 1];
						string result = "";

						// 파이프 fd를 닫는다.
						//close(pipe_fd);	// 이게 없으면 EOF검출이 안된다?

						// read
						while((ret = read(pipe_fd, buf, BUF_SIZE)) > 0 && strlen(buf) != 0) {
							buf[ret] = '\0';
							printf("[DEBUG]%s[ret:%d, len:%lu Loop...]\n", buf, ret, strlen(buf));
							string temp(buf);
							result += temp;
						}
						printf("[DEBUG]%s[ret:%d, len:%lu Loop...]\n", buf, ret, strlen(buf));
						cout << "[DEBUG]result: " << result << endl;

						// Client의 Response 객체 생성하기
						// string protocol = "HTTP/1.0 200 OK\r\n";
						// string servName = "Server:simple web server\r\n";
						// string cntLen = "Content-length:2048\r\n";
						// string cntType = "Content-type:text/html; charset=UTF-8\r\n\r\n";
						// string content = "<html><head><title>Default Page</title></head><body>" + result + "</body></html>";
						// string response = protocol+servName+cntLen+cntType+content;


						// 요청데이터 string을 응답데이터 string으로 교체
						clients[client_fd] = response;

						// ㅋ,ㅡㄹ라 write event activate
						change_events(change_list, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

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

							// åClient *c_ptr = new Client(client_socket, client_addr, client_len, *(new Request(clients[curr_event->ident])));
							// cout << "[DEBUG]" << *c_ptr << std::endl;
							//const char *res = make_response(*c_ptr, response);
							// make_response(*c_ptr, response);
							const char *res = clients[curr_event->ident].c_str();
							std::cout << "response: " << res << std::endl;
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