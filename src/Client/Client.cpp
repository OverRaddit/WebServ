#include "Client.hpp"

Client::Client()
	: fd(-1), len(0), req(NULL), pipe_fd(-1), raw_request(""), m_pending(false)
{
	//addr = -1;
}

Client::Client(int fd, sockaddr_in addr, socklen_t len)
	: fd(fd), addr(addr), len(len), req(NULL), pipe_fd(-1), raw_request("")
		, m_pending(false)
{
	//std::cout << "Client Constructor!" << std::endl;
}

Client::~Client()
{
	std::cout << "Client Destructor!" << std::endl;
	if (req)
		delete req;
	if (res)
		delete res;
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
void		Client::setResponse(Response *_res){ res = _res; }
void		Client::setRawRequest(std::string str){ raw_request = str; }

void		Client::appendRawRequest(std::string _raw_request){ raw_request += _raw_request; }

// 수정 필요!! 한번에 읽지않는경우.
int			Client::read_client_request()
{
	char buf[65524] = {0};
	int n = read(fd, buf, 65524 - 1);	// null문자까지 포함해서 읽기위해.
	if (n < 0)
	{
		std::cerr << "client read error!" << std::endl;
		return (-1);
	}
	else if (n == 0)
	{
		std::cout << "client just disconnect socket\n" << std::endl;
		return (-1);
	}
	// 그외 데이터 저장
	else
	{
		buf[n] = '\0';
		// if (n <= 3 && (buf[0] == '\n' || buf[0] == '\r'))
		// 	return 0;
		// 1개의 HTTP Request 읽기가 끝나면 동작시켜야 함.
		if (m_pending == false)
		{
			if (buf[0] < 'A' || buf[0] > 'Z')
				return 0;
			setRawRequest(string(buf, n));
			std::cout << "====== Request start ======" << std::endl;
			std::cout << "[" << getRawRequest() << "]" << std::endl;
			std::cout << "====== Request end ======" << std::endl;
			Request *req = new Request(getRawRequest());
			setRequest(req);
			cout << "Content-Length: " << req->getContentLength() << " Len :" << req->getReqBody().length() << endl;
		}
		// ㄴㅏ머지 요청을 읽습니다.
		else
		{
			std::cout << "====== ing Request start ======" << std::endl;
			std::cout << string(buf, n) << std::endl;
			std::cout << "====== Request end ======" << std::endl;
			if (req->getIsIncomplete())
			{
				string msg = req->getIncompleteMessage();
				cout << "Incomplete Message : " << msg << endl;
				msg.append(string(buf, n));
				cout << "Append Message : " << msg << endl;
				req->saveRequestAgain(msg);
				cout << "---------------------" << endl;
			}
			else if (req->saveOnlyBody(string(buf, n)) == req->getContentLength())
				m_pending = false;
		}

		// 현재 요청이 완성되었는지 확인한다. 1571,
		if (req->getContentLength() > req->getReqBody().length() || req->getIsIncomplete())
		{
			std::cout << "request uncomplete\n";
			m_pending = true;
			// 쪼개진 요청에 응답을 안한다.
			return 0;
		}
		else
			m_pending = false;

		// int ret;
		// if ((ret = cgi_init()) < 0)
		// 	return -1;
		// return ret;
		return 1;
	}
	return 0;
}

// 파이프fd, 클라이언트fd,
int			Client::read_pipe_result()
{
	int ret;
	char buf[65524];
	std::string result = "";

	// read
	// 비동기방식으로 바꿔야 함.
	while((ret = read(getPipeFd(), buf, 65524 - 1)) > 0 && strlen(buf) != 0) {
		buf[ret] = '\0';
		std::string temp(buf);
		result += temp;
	}

	std::cout << "====== pipe result start ======" << std::endl;
	std::cout << result << std::endl;
	std::cout << "====== pipe result end ======" << std::endl;

	// Client의 Response 객체 생성하기
	if (!req)
	{
		std::cout << "Request gone....!\n";
		return (0);
	}

	// CGI의 응답을 한번에 읽지 못하면 문제가 발생할 수 있다.
	// if (result.length() != 1)
	// {
	// 	return 0;
	// }

	res = new Response(req->getStatusCode());
	// 파일 다운로드 응답인 경우에 아래 헤더 추가
	//res->setHeaders("Content-Disposition", "attachment; filename=\"cool.html\"");
	res->cgiResponse(result);  // cgi 응답인 경우

	// 파일 업로드 요청인 경우
	//res->uploadResponse(req->getReqHeaderValue("Content-Type"), req->getReqBody());
	//res->downloadResponse(req->getReqBody());
	//res->deleteResponse(req->getReqBody());

	// 요청이 완전하고 upload 요청일때만 처리한다
	if (m_pending == false && req->getReqTarget() == "/upload")
	{
		std::cout << "Before upload Response\n";
		res->uploadResponse(req->getReqHeaderValue("Content-Type"), req->getReqBody());
		std::cout << "After upload Response\n";
	}

	// 파이프 종료는 서버에서 담당한다.
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
