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
#include "Server/Server.hpp"

int main(int argc, char *argv[])
{
	Server server;

	server.init_socket(atoi(argv[1]));
	server.init_multiplexing();
	server.run();
}
