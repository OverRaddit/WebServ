#include "Config.hpp"
#include <sys/_types/_size_t.h>

/**
 * Config 클래스의 기본 생성자입니다.
 * 필요한 경우 기본 설정을 초기화합니다.
 */
Config::Config(void) {}

/**
 * Config 클래스를 초기화하고 설정 파일을 읽어옵니다.
 * @param file 설정 파일의 경로입니다.
 * @brief 설정 파일을 읽고, 각 줄을 처리하여 설정 정보를 저장합니다.
 * 파일을 열 수 없는 경우 프로그램을 종료합니다.
 */
Config::Config(string file) {
	string	line = "";
	string	loc_block = "";
	bool		loc_block_flag = false;
	int			idx = -1;

	// config파일 open
	this->m_config_file.open(file);
	if (this->m_config_file.fail())
	{
		cerr << "Error\n";
		exit(1);
	}

	// 파싱
	while (getline(this->m_config_file, line))
	{
		if (line == "server {")
		{
			this->m_server_blocks.push_back(ServerBlock());
			idx++;
		}
		else if (!line.empty() && line != "}")
		{
			if (line.find("location") != string::npos)
				loc_block_flag = true;
			if (loc_block_flag)
			{
				loc_block += line;
				if (line == "\t}")
					loc_block_flag = false;
				if (!loc_block_flag)
				{
					this->m_server_blocks[idx].setLocationBlock(loc_block);
					loc_block = "";
				}
				loc_block += "\n";
			}
			else
				this->saveDirective(idx, line);
		}
	}
}

void Config::parseDirectiveLine(const std::string& line, std::string& directive, std::string& dir_data) const {
	size_t spacePos = line.find(' ');

	if (line.back() != ';')
	{
		cerr << "Sentence Must Be Finished By ';'\n";
		exit(1);
	}

	if (spacePos != std::string::npos) {
		directive = line.substr(1, spacePos - 1); // 맨앞글자인 /t를 제거하기 위해 1에서 시작.
		dir_data = line.substr(spacePos + 1, line.length() - (spacePos + 1) - 1); // 맨뒤의 ;를 제거하기 위해 -1
	} else {
		cerr << "Invalid directive format\n";
		exit(1);
	}
}

/**
 * 서버 블록 설정에 지시어를 저장합니다.
 * @param idx 지시어가 속한 서버 블록의 인덱스입니다.
 * @param line 지시어가 포함된 설정 줄입니다.
 * @brief 줄을 구문 분석하여 해당 서버 블록 설정에 할당합니다.
 * 줄이 세미콜론으로 끝나지 않는 경우 프로그램을 종료합니다.
 */
void	Config::saveDirective(int idx, string line)
{
	string	directive = "";
	string	dir_data = "";

	parseDirectiveLine(line, directive, dir_data);
	this->m_server_blocks[idx].setDirective(directive, dir_data);
}

/**
 * 서버 블록 목록을 가져옵니다.
 * @return ServerBlock 객체들의 벡터에 대한 참조를 반환합니다.
 */
vector<ServerBlock> &Config::getServerBlocks(void)
{
	return this->m_server_blocks;
}
