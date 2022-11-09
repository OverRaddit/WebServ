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
	else
	{
		buf[n] = '\0';
		if (m_pending == false)
		{
			if (buf[0] < 'A' || buf[0] > 'Z')
				return 0;
			setRawRequest(string(buf, n));
			Request *req = new Request(getRawRequest());
			setRequest(req);
		}
		else
		{
			if (req->getIsIncomplete())
			{
				string msg = req->getIncompleteMessage();
				msg.append(string(buf, n));
				req->saveRequestAgain(msg);
				appendRawRequest(msg);
			}
			else if (req->saveOnlyBody(string(buf, n)) == req->getContentLength())
				m_pending = false;
		}

		// 현재 요청이 완성되었는지 확인한다. 1571,
		if (req->getContentLength() > req->getReqBody().length() || req->getIsIncomplete())
		{
			m_pending = true;
			return 0;
		}
		else
		{
			m_pending = false;
			std::cout << "====== Request start ======" << std::endl;
			std::cout << "[" << getRawRequest() << "]" << std::endl;
			std::cout << "====== Request end ======" << std::endl;
		}
		return 1;
	}
	return 0;
}

// 파이프fd, 클라이언트fd,
int			Client::read_pipe_result()
{
	int ret;
	char buf[65524];

	ret = read(getPipeFd(), buf, 65524 - 1);	// 마지막에 NULL을 넣어야 seg fault 방지가능
	if (ret > 0)
	{
		buf[ret] = '\0';
		std::string temp(buf, ret);
		getRequest()->setCgiResult(getRequest()->getCgiResult() + temp);
	}
	else if (ret == 0)
		std::cout << "Read Pipe Done!" << std::endl;
	else
		std::cerr << "Read Error!!" << std::endl;

	if (ret != 0)
		return (0);

	std::cout << "====== pipe result start ======" << std::endl;
	std::cout << getRequest()->getCgiResult() << std::endl;
	std::cout << "====== pipe result end ======" << std::endl;

	res->cgiResponse(getRequest()->getCgiResult());
	return (1);
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

int			Client::cgi_init(string filepath)
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
	fcntl(to_parent[1], F_SETFL, O_NONBLOCK);
	fcntl(to_parent[0], F_SETFL, O_NONBLOCK);

	// open file
	int inputFile;
	if ((inputFile = open(filepath.c_str(), O_RDONLY)) < 0)
	{
		std::cerr << "cannot open input file of cgi" << std::endl;
		return -1;
	}

	// 자식(CGI)가 가져갈 표준입력 준비.
	char buf[BUF_SIZE + 1];
	memset(buf, 0, sizeof(buf));
	int ret = -1;
	while((ret = read(inputFile, buf, sizeof(buf))) >= 0)
	{
		buf[ret] = 26; // EOF
		if (ret < 0)
			return -1;
		if (ret == 0)
			break;
		if (write(to_child[1], buf, strlen(buf)) < 0)
			return -1;
		memset(buf, 0, sizeof(buf));
	}
	if (write(to_child[1], buf, strlen(buf)) < 0)
		return -1;

	pid = fork();
	getRequest()->setCgiPid(pid);
	if (pid == 0)
	{
		dup2(to_child[0], 0);
		dup2(to_parent[1], 1);
		close(to_child[1]);
		close(to_child[0]);
		close(to_parent[1]);
		close(to_parent[0]);
		if (execve("./src/cgi_tester", 0, env) == -1) {
			std::cerr << "[child]cgi error\n";
			exit(1);
		}
	}
	close(to_child[0]);
	close(to_child[1]);
	close(to_parent[1]);
	for (int i=0; i<10; ++i)
		free(env[i]);
	free(env);
	return to_parent[0];
}
