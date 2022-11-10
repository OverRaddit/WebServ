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
			res->makeContent("No such file! Can't get cgi File");
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
			cout << "This file is directory. Return IndexFile : " << req->getLocBlock().getRootDir() + req->getReqFileName() + "/" + req->getLocBlock().getIndexFile() << endl;
			// 이 경우, index file은 항상 존재한다고 가정한다.
			target = req->getLocBlock().getRootDir() + req->getReqFileName() + "/" + req->getLocBlock().getIndexFile();
			break;
	}

	// ReqFileName에 cgi 확장자 존재시, cgi처리를 한다.
	// (1)응답을 cgi에 넣어서 새로운 응답을 만들까.
	// (2)파일을 cgi에 넣은 결과를 응답으로 할까.
	// 적고보니 (2)가 좋아보임.
	// 그러면, 응답파일이 index여도 똑같이 CGI로 처리해야 할까?
	res->setStatusCode(200);
	if (is_cgi_request(req))
	{
		if (cgi_init(target) < 0)
			std:cerr << "CGI ERROR" << std::endl;
	}
	else
	{
		res->fileResponse(target);
	}
	return 0;
}

// 현재 uploadResponse가 파일명을 받지 않음
int Client::POST(Request *req, Response *res, string filepath)
{
	cout << "[POST]" << endl;

	int fileflag = res->getRequestFile(req->getLocBlock().getRootDir(), req->getReqFileName());
	string target = "";

	if (is_cgi_request(req))
	{
		if (cgi_init(req->getReqBody()) < 0)
			std:cerr << "CGI ERROR" << std::endl;
		return 0;
	}

	switch(fileflag)
	{
		case NO_FILE:
			cout << "Upload New File" << endl;
			//if fail,, other status code
			res->setStatusCode(201);
			break;
		case VALID_REQ_FILE:
			cout << "File already Exist! Truc File..." << endl;
			target = req->getLocBlock().getRootDir() + req->getReqFileName();
			res->setStatusCode(201);
			break;
		case VALID_REQ_DIR:
			cout << "Request file is directory. you can't change Index File!" << endl;
			res->errorResponse(404);
			return 0;
	}

	res->uploadResponse(target, req->getReqHeaderValue("Content-Type"), req->getReqBody());
	res->fileResponse(target);
	return 0;
}

int Client::DELETE(Request *req, Response *res, string filepath)
{
	cout << "[DELETE]" << endl;

	int fileflag = res->getRequestFile(req->getLocBlock().getRootDir(), req->getReqFileName());
	string finalpath = "";

	if (is_cgi_request(req))
	{
		if (cgi_init(req->getReqBody()) < 0)
			std:cerr << "CGI ERROR" << std::endl;
		return 0;
	}

	switch (fileflag)
	{
		case NO_FILE:
			cout << "Cannot found File" << endl;
			res->makeContent("No such file");
			res->errorResponse(404);
			break;
		case VALID_REQ_FILE:
			cout << "Found File! delete this file..." << endl;
			res->deleteResponse(filepath);
			res->setStatusCode(204);
			break;
		case VALID_REQ_DIR:
			cout << "This file is directory. you can't delete indexfile!" << req->getLocBlock().getRootDir() + "/" + req->getLocBlock().getIndexFile() << endl;
			res->errorResponse(404);
			break;
	}
	return 0;
}
