#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <fcntl.h>
using namespace std;

int main()
{
	if (execve("./temp/hello", 0, 0) == -1) {
			std::cout << "cgi error\n";
			return -1;
	}
}
