//#include "Server.hpp"
#include "Client.hpp"

bool Client::is_cgi_request(Request *req)
{
	return (req->getReqFileName().find(req->getSerBlock().getCgiExtension()) != string::npos);
}

//============================================

int Client::GET(Request *req, Response *res, string filepath)
{
	cout << "[GET]" << endl;

	//  debug path
	int fileflag = res->getRequestFile(req->getReqFileName(), req->getLocBlock().getRootDir());
	cout << "rootdir: [" << req->getLocBlock().getRootDir() << "]" << endl;
	cout << "req file name: [" << req->getReqFileName() << "]" << endl;
	string target = "";

	// file status
	switch(fileflag)
	{
		case NO_FILE:
			cout << "Cannot found File" << endl;
			res->makeContent("No such file");
			res->errorResponse(404);
			return 0;
		case VALID_REQ_FILE:
			cout << "Found File" << endl;
			target = req->getLocBlock().getRootDir() + req->getReqFileName();
			break;
		case VALID_REQ_DIR:
			// if (file_name.back() == '/')
			// 	final_path = dir_path + file_name;
			// else
			// 	final_path = dir_path + file_name + "/";
			cout << "This file is directory. Return IndexFile : " << req->getLocBlock().getRootDir() + "/" + req->getLocBlock().getIndexFile() << endl;
			// 이 경우, index file은 항상 존재한다고 가정한다.
			target = req->getLocBlock().getRootDir() + "/" + req->getLocBlock().getIndexFile();
			break;
	}

	// ReqFileName에 cgi 확장자 존재시, cgi처리를 한다.
	// (1)응답을 cgi에 넣어서 새로운 응답을 만들까.
	// (2)파일을 cgi에 넣은 결과를 응답으로 할까.
	// 적고보니 (2)가 좋아보임.
	// 그러면, 응답파일이 index여도 똑같이 CGI로 처리해야 할까?
	if (is_cgi_request(req))
	{
		if (cgi_init(target) < 0)
			std:cerr << "CGI ERROR" << std::endl;
	}
	else
		res->fileResponse(target);
	res->setStatusCode(200);
	return 0;
}

// 현재 uploadResponse가 파일명을 받지 않음
int Client::POST(Request *req, Response *res, string filepath)
{
	// cout << "[POST]" << endl;

	// int fileflag = res->getRequestFile(req->getLocBlock().getRootDir(), req->getReqFileName());
	// string finalpath = "";

	// switch(fileflag)
	// {
	// 	case NO_FILE:
	// 		cout << "Upload New File" << endl;
	// 		res->uploadResponse(req->getReqHeaderValue("Content-Type"), req->getReqBody());
	// 		//if fail,, other status code
	// 		res->setStatusCode(201);
	// 		break;
	// 	case VALID_REQ_FILE:
	// 		cout << "File already Exist! Truc File..." << endl;
	// 		// 파일 열기모드를 다른걸로 해야함.
	// 		res->uploadResponse(req->getReqHeaderValue("Content-Type"), req->getReqBody());
	// 		res->setStatusCode(201);
	// 		break;
	// 	case VALID_REQ_DIR:
	// 		cout << "Request file is directory. Truc IndexFile : " << req->getLocBlock().getRootDir() + "/" + req->getLocBlock().getIndexFile() << endl;
	// 		// 이 경우, index file은 항상 존재한다고 가정한다.
	// 		// 파일 열기모드를 다른걸로 해야함.
	// 		res->uploadResponse(req->getReqHeaderValue("Content-Type"), req->getReqBody());
	// 		res->setStatusCode(201);
	// 		break;
	// }

	// if (is_cgi_request(req))
	// {
	// 	//???????????????
	// 	//if (cgi_init(target) < 0)
	// 	if (cgi_init(req->getReqBody()) < 0)
	// 		std:cerr << "CGI ERROR" << std::endl;
	// }
	// else
	// 	res->fileResponse(target);
	// res->setStatusCode(200);
	// return 0;
}

int Client::DELETE(Request *req, Response *res, string filepath)
{
	// cout << "[DELETE]" << endl;

	// int fileflag = res->getRequestFile(req->getLocBlock().getRootDir(), req->getReqFileName());
	// string finalpath = "";

	// switch(fileflag)
	// {
	// 	case NO_FILE:
	// 		cout << "Cannot found File" << endl;
	// 		res->makeContent("No such file");
	// 		res->errorResponse(404);
	// 		break;
	// 	case VALID_REQ_FILE:
	// 		cout << "Found File" << endl;
	// 		res->deleteResponse(filepath);
	// 		res->setStatusCode(200);
	// 		break;
	// 	case VALID_REQ_DIR:
	// 	// index 파일이 삭제되는건 말이안되는데...?
	// 		cout << "This file is directory. you can't delete indexfile!" << req->getLocBlock().getRootDir() + "/" + req->getLocBlock().getIndexFile() << endl;
	// 		res->setStatusCode(404);
	// 		res->errorResponse(-1); // 인자값 의미를 모르겠음.
	// 		break;
	// }

	// // if (is_cgi_request(req))
	// // {

	// // }
	// return 0;
}