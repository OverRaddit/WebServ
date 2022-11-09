#include <iostream>
#include "./Response.hpp"

int main() {
	Response res;
	res.autoIndexResponse("../../sudo/file_storage");
	cout << res.getHttpResponse() << "\n";
	return 0;
}
