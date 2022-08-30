#ifndef CGI_H
# define CGI_H

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <vector>

using namespace std; // <- subject에서 허용했는지?
void split(const string& str, const string& delimiters , vector<string>& tokens);

#endif
