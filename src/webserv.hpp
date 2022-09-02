#ifndef CGI_H
# define CGI_H

// Server side C program to demonstrate Socket programming
# include <sys/socket.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/event.h>
# include <err.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include <map>
# include <vector>
# include <iostream>
# include <cstring>
# include <string>

# include "../include/get_next_line/get_next_line.h"
# include "./Request/Request.hpp"
# include "./Client/Client.hpp"


# define BUF_SIZE 1024

using namespace std; // <- subject에서 허용했는지?
void split(const string& str, const string& delimiters , vector<string>& tokens);

#endif
