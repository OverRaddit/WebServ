#include "cgi.hpp"

void make_env(char **env) {
	env[0] = strdup("AUTH_TYPE=");
	env[1] = strdup("CONTENT_LENGTH=\0");
	env[2] = strdup("CONTENT_TYPE=\0");
	env[3] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[4] = strdup("PATH_INFO=/bye");
	env[5] = strdup("PATH_TRANSLATED=index.html");	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑. 요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑.
	env[6] = strdup("QUERY_STRING=");	// 경로 뒤의 요청 URL에 포함된 조회 문자열.
	env[7] = strdup("REMOTE_ADDR=127.0.0.1");	// 요청을 보낸 클라이언트 IP 주소.
	env[8] = strdup("REMOTE_IDENT=");	// Identification. 클라이언트에서 GCI 프로그램을 실행시킨 사용자.
	env[9] = strdup("REMOTE_USER=");	// 사용자가 인증된 경우 이 요청을 작성한 사용자의 로그인을 의미.	null (인증되지 않음)
	env[10] = strdup("REQUEST_METHOD=GET");	// 요청 HTTP 메소드 이름. (GET, POST, PUT)
	env[11] = strdup("REQUEST_URI=/cgi/bye");	// 현재 페이지 주소에서 도메인을 제외한 값.
	env[12] = strdup("SCRIPT_NAME=cgi");	// HTTP 요청의 첫 번째 라인에 있는 조회 문자열까지의 URL.
	env[13] = strdup("SERVER_NAME=webserv");	// 요청을 수신한 서버의 호스트 이름.
	env[14] = strdup("SERVER_PORT=4242");	// 요청을 수신한 서버의 포트 번호.
	env[15] = strdup("SERVER_PROTOCOL=HTTP/1.1");	// 요청이 사용하는 프로토콜의 이름과 버전. 	protocol/majorVersion.minorVersion 양식
	env[16] = strdup("SERVER_SOFTWARE=");	// 서블릿이 실행 중인 컨테이너의 이름과 버전.
	env[17] = 0;
}

char buf[1024];

int main(int argc, char **argv, char **envp) {
	pid_t pid;
	int m_pipe[2];
	int status;
	char **env;
	char *str = strdup("./cgi_tester");

	env = (char**)malloc(sizeof(char*) * 18);
	make_env(env);
	//execve(str, str2, env);
	pipe(m_pipe);
	pid = fork();
	//fcntl(m_pipe[1], F_SETFL, O_NONBLOCK);
	//fcntl(m_pipe[0], F_SETFL, O_NONBLOCK);
	if (pid == 0) {
		dup2(m_pipe[1], 1);
		close(m_pipe[1]);
		close(m_pipe[0]);
		if (execve(str, 0, env) == -1) {
			std::cout << "cgi error\n";
			return -1;
		}
	} else {
		dup2(m_pipe[0], 0);
		close(m_pipe[1]);
		close(m_pipe[0]);
		//fcntl(0, F_SETFL, O_NONBLOCK);
		waitpid(pid, &status, 0);
		while((status = read(0, buf, 1024)) > 0) {
			write(1, buf, status);
		}
	}
	//while (*env) {
	//	std::cout << *env << "\n";
	//	env++;
	//}
	return 0;
}
