#include "Config/LocationBlock.hpp"
#include "Config/ServerBlock.hpp"

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
#include "Config/Config.hpp"
#include "Server/Server.hpp"

int main(int argc, char *argv[])
{
	Server server(argv[1]);

	if (argc == 2)
	{
		//Config	config(argv[2]);
		//server.init_socket();
		server.init_multiplexing();
		server.run();
		// config 파일 파싱 체크
		// vector<ServerBlock> s_b = config.getServerBlocks();
		// for (int i = 0;i < s_b.size();i++)
		// {
		// 	int port_num = s_b[i].getPortNum();
		// 	string	err_file_path = s_b[i].getErrorPage(404);
		// 	string	server_name = s_b[i].getServerName();
		// 	string	root_dir = s_b[i].getRootDir();
		// 	string	cgi_tester = s_b[i].getCgiTester();
		// 	string	index_file = s_b[i].getIndexFile();
		// 	map<string, LocationBlock> l_b = s_b[i].getLocationBlocks();

		// 	cout << "Port Num : " << port_num << endl;
		// 	cout << "err_file_path : " << err_file_path << endl;
		// 	cout << "server_name : " << server_name << endl;
		// 	cout << "root_dir : " << root_dir << endl;
		// 	cout << "cgi_tester : " << cgi_tester << endl;
		// 	cout << "index_file : " << index_file << endl;
		// 	cout << "location : :" << l_b["/delete"].getValidMethod() << endl;
		// 	cout << "----------------------------------------------\n";
		// }
	}
}