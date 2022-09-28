/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gshim <gshim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/07 19:50:39 by gshim             #+#    #+#             */
/*   Updated: 2022/09/28 22:16:37 by gshim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//=============================================================================
//	Orthodox Canonical Form
//=============================================================================
Server::Server()
{
}
Server::Server(string config_filepath)
{
	// read config file.
	config = new Config(config_filepath);
}
Server::Server(const Server& a){ *this = a; }
Server& Server::operator=(const Server& a){ return *this; }
Server::~Server(){}

//=============================================================================
//	Method
//=============================================================================
int Server::init_socket(int port)
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
	//address.sin_port = htons(  );
	address.sin_port = htons( port );

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
	return (0);
}

// server_fd
int Server::init_multiplexing()
{
	struct kevent event;	 /* 감시대상 이벤트 */
	int flags = 0;
	int ret = 0;

	kq_fd = kqueue();
	if (kq_fd == -1)
		err(EXIT_FAILURE, "kqueue() failed");

	// flag에 0넣으면?
	EV_SET(&event, server_fd, EVFILT_READ, EV_ADD | EV_CLEAR, flags, 0,	NULL);

	ret = kevent(kq_fd, &event, 1, NULL, 0, NULL);
	if (ret == -1)
		err(EXIT_FAILURE, "kevent register");
	if (event.flags & EV_ERROR)
		errx(EXIT_FAILURE,	"Event error: %s", strerror(event.data));
	return (0);
}

int Server::run()
{
	change_events(server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "WebServer started" << std::endl;

	// 통신.
	int new_events;
	struct kevent* curr_event;
	for (;;)
	{
		// kevent 호출
		new_events = kevent(kq_fd, &change_list[0], change_list.size(), event_list, 8, NULL);
		// change_list 비우기
		change_list.clear();

		// 이벤트리스트순회
		for(int i=0;i < new_events ;i++)
		{
			curr_event = &event_list[i];

			if (curr_event->flags & EV_ERROR)
				callback_error(curr_event->ident);
			else if (curr_event->filter == EVFILT_READ)
				callback_read(curr_event->ident);
			else if (curr_event->filter == EVFILT_WRITE)
				callback_write(curr_event->ident);
		}
	}
}
