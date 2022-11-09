#include <iostream>
#include <fstream>
#include "./Response.hpp"

int main() {
	//Response res;
	//res.autoIndexResponse("../../sudo/file_storage");
	//cout << res.getHttpResponse() << "\n";
	ofstream f;
	f.open("ttt.txt");
	string content1 = "test content1\n";
	f.write(content1.c_str(), content1.size());
	//string content2 = "test content2\n";
	//f.write(content2.c_str(), content2.size());
	//string content3 = "test content3\n";
	//f.write(content3.c_str(), content3.size());
	f.close();
	return 0;
}
