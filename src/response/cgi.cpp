/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minskim2 <minskim2@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/28 17:13:48 by minskim2          #+#    #+#             */
/*   Updated: 2022/08/28 21:14:33 by minskim2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

static char **make_env() {
	char **env = (char**)malloc(sizeof(char*) * 18);
	if (!env)
		exit(1);
	env[0] = strdup("AUTH_TYPE=");
	env[1] = strdup("CONTENT_LENGTH=1000");
	env[2] = strdup("CONTENT_TYPE=html/text");
	env[3] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[4] = strdup("PATH_INFO=/cgi/bye");
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
	return env;
}

static void free_env(char **env) {
	for (int i=0; i<17; ++i) {
		free(env[i]);
	}
	free(env);
}

int run_cgi() {
	pid_t pid;
	int m_pipe[2];
	int status;
	char *str = strdup("./cgi_tester");
	char buf[1024] = {};
	char **env;

	env = make_env();
	pipe(m_pipe);
	pid = fork();
	//fcntl(m_pipe[1], F_SETFL, O_NONBLOCK);
	//fcntl(m_pipe[0], F_SETFL, O_NONBLOCK);
	if (pid == 0) {
		dup2(m_pipe[1], 1);
		close(m_pipe[1]);
		close(m_pipe[0]);
		if (execve(str, 0, env) == -1) {
			write(2, "cgi error\n", 10);
			return -1;
		}
	} else {
		dup2(m_pipe[0], 0);
		fcntl(0, F_SETFL, O_NONBLOCK);
		close(m_pipe[1]);
		close(m_pipe[0]);
		waitpid(pid, &status, WNOWAIT);
		//read()
		while ((status = read(0, buf, 1)) != 0) {
			if (status != -1)
				printf("status=%d\n", status);
			write(1, buf, status);
		}
	}
	free(str);
	free_env(env);
	return 0;
}
