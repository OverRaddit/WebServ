# Webserv

본 문서는 42Seoul inner circle(5circle) 과제를 수행하며 정리한 내용입니다.

## Webserv 과제이해

- Multiplexing을 적용하여 blocking I/O가 아닌 non-blocking I/O로 구현한다.

- Non-blocking fd를 사용하여 입출력시 blocking되지 않으므로 poll()류의 함수를 선행할것.

## 과제를 하며 궁금했던 개념

- 허용함수 원형 + 설명하기.

- Multiplexing
	- Blocking I/O vs Non-blocking I/O
	- 기본 socket 함수 in C language
	- select(), poll(), epoll(), kqueue() 류와 같은 소켓함수

- HTTP 1.1
	- HTTP Method
	- HTTP Chunked Message

- 정적 vs 동적 페이지
- [Web Server]
	- Web Server software ranking 2022를 검색하면..
	- NGinX
	- Apache HTTP Server
	- IIS
- Container(=CGI?)
- [Web Application Server] = Web Server + Container
	-

- CGI
	- Meta-variables

https://velog.io/@ayokinya/CGI
https://qaos.com/sections.php?op=viewarticle&artid=194
- static Website

## 구현사항.

- upload
	- html <form> 태그를 이용해 파일을 업로드 할 수 있게하면 좋을 것 같다.