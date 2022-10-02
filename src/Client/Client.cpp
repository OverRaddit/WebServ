#include "Client.hpp"

Client::Client()
	: fd(-1), len(0), req(NULL), pipe_fd(-1), raw_request("")
{
	//addr = -1;
}

Client::Client(int fd, sockaddr_in addr, socklen_t len)
	: fd(fd), addr(addr), len(len), req(NULL), pipe_fd(-1), raw_request("")
{
	//std::cout << "Client Constructor!" << std::endl;
}

Client::~Client()
{
	//std::cout << "Client Destructor!" << std::endl;
	if (req)
		delete req;
}

Client::Client(const Client& a)
: fd(a.fd), pipe_fd(a.pipe_fd), addr(a.addr), len(a.len), raw_request(a.raw_request)
{
	req = new Request(*(a.req));
}

Client& Client::operator=(const Client& a)
{
	fd = a.fd;
	pipe_fd = a.pipe_fd;
	addr = a.addr;
	len = a.len;
	raw_request = a.raw_request;
	req = new Request(*(a.req));

	return *this;
}

int	Client::getFd() const { return fd; }
int	Client::getPipeFd() const { return pipe_fd; }
sockaddr_in	Client::getAddr() const { return addr; }
socklen_t	Client::getLen() const { return len; }
Request*	Client::getRequest(){ return req; }
Response*	Client::getResponse() const { return res; }
std::string	Client::getRawRequest() const {return raw_request; }

void		Client::setPipeFd(int _pipe_fd){ pipe_fd = _pipe_fd; }
void		Client::setRequest(Request *_req){ req = _req; }
void		Client::setRawRequest(std::string str){ raw_request = str; }

void		Client::appendRawRequest(std::string _raw_request){ raw_request += _raw_request; }

// 수정 필요!! 한번에 읽지않는경우.
int			Client::read_client_request()
{
	char buf[1024] = {0};
	int n = read(fd, buf, sizeof(buf));
	// read 결과가 0미만일시 disconnect, 등호인 경우??
	if (n <= 0)
	{
		std::cerr << "client read error!" << std::endl;
		return (-1);
	}
	// // 자식프로세스 생성
	// else if (n == 0)	// 더 이상 읽을 데이터가 없는 경우 -> 요청을 모두 읽은 경우.
	// {
	// 	int ret;
	// 	std::cout << "[DEBUG]Method is " << getRequest()->getMethod() << std::endl;
	// 	if ((ret = cgi_init()) < 0)
	// 		return -1;
	// 	return ret;
	// }
	// 그외 데이터 저장
	else
	{
		buf[n] = '\0';
		appendRawRequest(buf);
		// Debug Reading
		// std::cout << "[DEBUG]received data from " << getFd() << ": " << getRawRequest() << std::endl;
		// std::cout << "[DEBUG] ret was " << n << std::endl;

		int ret;
		// 1개의 HTTP Request 읽기가 끝나면 동작시켜야 함.
		{
			Request *req = new Request(getRawRequest());
			setRequest(req);
			// 요청헤더에서 Host를 읽어 어떤 호스트의 몇번 포트에 접근하는지 확인한다.

			// 해당 정보로 적절한 서버블록을 꺼낸다.

			if ((ret = cgi_init()) < 0)
				return -1;

		}
		return ret;
	}
	return 0;
}

// 파이프fd, 클라이언트fd,
int			Client::read_pipe_result()
{
	int ret;
	char buf[BUF_SIZE + 1];
	std::string result = "";

	// read
	while((ret = read(getPipeFd(), buf, BUF_SIZE)) > 0 && strlen(buf) != 0) {
		buf[ret] = '\0';
		std::string temp(buf);
		result += temp;
	}

	// Client의 Response 객체 생성하기
	res = new Response();
	Response::ResponseInit();
	res->setStatusCode(req->getStatusCode());
	res->setHeaders("Content-type", "text/html; charset=UTF-8");
	// 파일 다운로드 응답인 경우에 아래 헤더 추가
	//res->setHeaders("Content-Disposition", "attachment; filename=\"cool.html\"");
	res->makeContent(result);

	close(pipe_fd);
	return (0);
}

void		Client::make_env(char **env)
{
	env[0] = strdup(("AUTH_TYPE=" + req->getReqHeaderValue("Authorization")).c_str());
	env[1] = strdup(("CONTENT_LENGTH=" + req->getReqHeaderValue("Content-Length")).c_str());  // 요청 길이를 알 수 없는경우 -1 이여야함
	env[2] = strdup(("CONTENT_TYPE=" + req->getReqHeaderValue("Content-Type")).c_str());
	env[3] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[4] = strdup(("PATH_INFO=" + req->getReqTarget()).c_str());
	//env[5] = strdup(("PATH_TRANSLATED=");	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑. 요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑.
	//env[6] = strdup("QUERY_STRING=");	// 경로 뒤의 요청 URL에 포함된 조회 문자열.
	//env[7] = strdup("REMOTE_ADDR=");	// 요청을 보낸 클라이언트 IP 주소.
	//env[8] = strdup("REMOTE_IDENT=");	// Identification. 클라이언트에서 GCI 프로그램을 실행시킨 사용자.
	//env[9] = strdup("REMOTE_USER=");	// 사용자가 인증된 경우 이 요청을 작성한 사용자의 로그인을 의미.	null (인증되지 않음)
	env[5] = strdup(("REQUEST_METHOD=" + req->getMethod()).c_str());	// 요청 HTTP 메소드 이름. (GET, POST, PUT)
	env[6] = strdup(("REQUEST_URI=" + req->getReqTarget()).c_str());	// 현재 페이지 주소에서 도메인을 제외한 값.
	env[7] = strdup("SCRIPT_NAME=cgi");	// HTTP 요청의 첫 번째 라인에 있는 조회 문자열까지의 URL.
	env[8] = strdup("SERVER_NAME=webserv");	// 요청을 수신한 서버의 호스트 이름.
	//env[9] = strdup("SERVER_PORT=4242");	// 요청을 수신한 서버의 포트 번호.
	env[9] = strdup("SERVER_PROTOCOL=HTTP/1.1");	// 요청이 사용하는 프로토콜의 이름과 버전. 	protocol/majorVersion.minorVersion 양식
	//env[16] = strdup("SERVER_SOFTWARE=");	// 서블릿이 실행 중인 컨테이너의 이름과 버전.
	env[10] = 0;
}

int			Client::cgi_init()
{
	pid_t	pid;
	int		to_child[2];
	int		to_parent[2];

	pipe(to_child);
	pipe(to_parent);
	setPipeFd(to_parent[0]);

	char **env;
	env = (char**)malloc(sizeof(char*) * 11);
	make_env(env);
	// 파이프 fd를 nonblock하면 어떻게 되는 거지?
	// fcntl(m_pipe[1], F_SETFL, O_NONBLOCK);
	// fcntl(m_pipe[0], F_SETFL, O_NONBLOCK);

	// 자식(CGI)가 가져갈 표준입력 준비.
	// 버퍼 한번에 담을 수 없는 양이 들어오면 어떡해야 할 지 모르겠다.
	char buf[BUF_SIZE + 1];
	memset(buf, 0, sizeof(buf));
	char *body = strdup(getRequest()->getReqBody().c_str()); // 왜 warning?
	memcpy(buf, body, strlen(body));
	buf[strlen(body)] = 26;	// EOF값을 준다.
	//write(to_child[1], buf, sizeof(buf)); // 3번째 인자를 strlen(buf)로 해야하나?
	write(to_child[1], buf, strlen(buf)); // 3번째 인자를 strlen(buf)로 해야하나?

	pid = fork();
	if (pid == 0)
	{	// child
		dup2(to_child[0], 0);	// 부모->자식파이프의 읽기fd == 자식의 표준입력
		dup2(to_parent[1], 1);	// 자식->부모파이프 쓰기fd == 자식의 표준출력
		close(to_child[1]);
		close(to_child[0]);
		close(to_parent[1]);
		close(to_parent[0]);
		if (execve("./src/cgi_tester", 0, env) == -1) {
			std::cerr << "[child]cgi error\n";
			exit(1);
		}
	}
	// parent
	close(to_child[0]);
	close(to_child[1]);
	close(to_parent[1]);
	free(body);
	for (int i=0; i<10; ++i)
		free(env[i]);
	free(env);

	// kqueue에 파이프 등록해야 함.
	return to_parent[0];
}
