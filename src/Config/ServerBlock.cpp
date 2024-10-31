#include "ServerBlock.hpp"

/**
 * ServerBlock 클래스의 기본 생성자입니다.
 * 초기화 작업을 수행합니다.
 */
ServerBlock::ServerBlock(void): m_error_page("")
{}

/**
 * 지시어 이름에 따라 해당 설정 메소드를 호출하여 데이터를 설정합니다.
 * @param directive 지시어의 이름입니다.
 * @param data 지시어에 따른 설정 값입니다.
 * @note 지정되지 않은 지시어가 입력된 경우 프로그램을 종료합니다.
 */
void ServerBlock::setDirective(const std::string& directive, const std::string& data)
{
	if (directive == "listen")
		this->setPortNums(data);
	else if (directive == "server_name")
		this->setServerName(data);
	else if (directive == "root")
		this->setRootDir(data);
	else if (directive == "cgi")
		this->setCgiTester(data);
	else if (directive == "error_page")
		this->setErrorPage(data);
	else if (directive == "index")
		this->setIndexFile(data);
	else {
		cerr << "지정되지 않은 지시어가 발견되었습니다. : [" << directive << "]\n";
		exit(1);
	}
}

/**
 * location 블록을 설정합니다.
 * @param loc_block 설정할 location 블록의 내용입니다.
 */
void	ServerBlock::setLocationBlock(string loc_block)
{
	size_t	pos;
	string	route = "";

	pos = loc_block.find("/");
	for (size_t i = pos;loc_block[i] != ' ';i++)
		route += loc_block[i];
	this->m_loc_blocks[route] = LocationBlock(this->m_root_dir, this->m_index_file, this->m_error_page);
	if ((pos = loc_block.find("\tlimit_except")) != string::npos)
		this->m_loc_blocks[route].setValidMethod(loc_block, pos + 1);
	if ((pos = loc_block.find("\tclient_max_body_size")) != string::npos)
		this->m_loc_blocks[route].setMaxBodySize(loc_block, pos + 1);
	if ((pos = loc_block.find("\tupload")) != string::npos)
		this->m_loc_blocks[route].setUploadDirectory(loc_block, pos + 1);
	if ((pos = loc_block.find("\tautoindex")) != string::npos)
		this->m_loc_blocks[route].setAutoIndex(loc_block, pos + 1);
	if ((pos = loc_block.find("\troot")) != string::npos)
		this->m_loc_blocks[route].setRootDir(loc_block, pos + 1);
	if ((pos = loc_block.find("\treturn")) != string::npos)
		this->m_loc_blocks[route].setRedirectionURL(loc_block, pos + 1);
	if ((pos = loc_block.find("\tindex")) != string::npos)
		this->m_loc_blocks[route].setIndexFile(loc_block, pos + 1);
}

/**
 * 오류 페이지 정보를 설정합니다.
 * @param error_page_info 오류 페이지 경로입니다.
 */
void	ServerBlock::setErrorPage(string error_page_info) { this->m_error_page = error_page_info; }

/**
 * 인덱스 파일명을 설정합니다.
 * @param file_name 인덱스 파일의 이름입니다.
 */
void	ServerBlock::setIndexFile(string file_name){ this->m_index_file = file_name; }

/**
 * CGI 테스트 프로그램과 확장자를 설정합니다.
 * @param cgi_tester CGI 프로그램 경로와 확장자 정보입니다.
 */
void	ServerBlock::setCgiTester(string cgi_tester)
{
	bool flag = false;

	this->m_cgi_tester = "";
	this->m_cgi_extension = "";
	for (size_t i = 0;i < cgi_tester.length();i++)
	{
		if (cgi_tester[i] == ' ')
		{
			flag = true;
			continue;
		}
		if (!flag)
			this->m_cgi_tester += cgi_tester[i];
		else
			this->m_cgi_extension += cgi_tester[i];
	}
}

/**
 * 루트 디렉토리를 설정합니다.
 * @param root_dir 루트 디렉토리 경로입니다.
 */
void	ServerBlock::setRootDir(string root_dir){ this->m_root_dir = root_dir; }

/**
 * 서버 이름을 설정합니다.
 * @param server_name 서버의 이름입니다.
 */
void	ServerBlock::setServerName(string server_name) { this->m_server_name = server_name; }

/**
 * 포트 번호 목록을 설정합니다.
 * @param port_nums 포트 번호를 나타내는 문자열입니다.
 * @note 잘못된 포트 번호 또는 중복된 포트가 입력된 경우 프로그램을 종료합니다.
 */
void	ServerBlock::setPortNums(string port_nums)
{
	string	port_num = "";
	bool 	flag = false;
	int		int_port_num = 0;

	if (port_nums == "")
	{
		cerr << "LISTEN PORT IS EMPTY\n";
		exit(1);
	}
	for (size_t i = 0;i < port_nums.length();i++)
	{
		if (port_nums[i] != ' ')
		{
			if (port_nums[i] < '0' || port_nums[i] > '9')
			{
				cerr << "INVALID PORT NUMBER\n";
				exit(1);
			}
			port_num += port_nums[i];
		}
		else
			flag = true;
		if (flag || i == port_nums.length() - 1)
		{
			try
			{
				int_port_num = stoi(port_num);
				for (size_t i = 0;i < this->m_port_nums.size();i++)
				{
					if (this->m_port_nums[i] == int_port_num)
					{
						cerr << "DUPLICATED LISTEN PORT FOUNDED!!\n";
						exit(1);
					}
				}
				this->m_port_nums.push_back(int_port_num);
			}
			catch(exception &e) {
				cerr << "INVALID PORT NUMBER\n";
				exit(1);
			}
			port_num = "";
			flag = false;
		}
	}
}

/**
 * 설정된 오류 페이지 경로를 반환합니다.
 * @return 오류 페이지 경로를 나타내는 문자열입니다.
 */
string	ServerBlock::getErrorPage(void) const { return this->m_error_page; }

/**
 * 설정된 인덱스 파일명을 반환합니다.
 * @return 인덱스 파일명을 나타내는 문자열입니다.
 */
string ServerBlock::getIndexFile(void) const { return this->m_index_file; }

/**
 * 설정된 CGI 프로그램 경로를 반환합니다.
 * @return CGI 프로그램 경로를 나타내는 문자열입니다.
 */
string ServerBlock::getCgiTester(void) const { return this->m_cgi_tester; }

/**
 * 설정된 CGI 확장자를 반환합니다.
 * @return CGI 확장자를 나타내는 문자열입니다.
 */
string ServerBlock::getCgiExtension(void) const { return this->m_cgi_extension; }

/**
 * 루트 디렉토리 경로를 반환합니다.
 * @return 루트 디렉토리 경로를 나타내는 문자열입니다.
 */
string ServerBlock::getRootDir(void) const { return this->m_root_dir; }

/**
 * 서버 이름을 반환합니다.
 * @return 서버 이름을 나타내는 문자열입니다.
 */
string ServerBlock::getServerName(void) const { return this->m_server_name; }

/**
 * 설정된 포트 번호 목록을 반환합니다.
 * @return 포트 번호 목록을 나타내는 정수 벡터입니다.
 */
vector<int>	ServerBlock::getPortNum(void) const { return this->m_port_nums; }

/**
 * LocationBlock의 맵을 반환합니다.
 * @return LocationBlock 객체들의 맵에 대한 참조입니다.
 */
map<string, LocationBlock>&	ServerBlock::getLocationBlocks(void) { return this->m_loc_blocks; }

