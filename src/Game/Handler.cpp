#include "Handler.hpp"
#include "GameData.hpp"
#include "../Loaders/BigStream.hpp"
#include "../Loaders/Vp6Stream.hpp"
#include <sstream>
#include <iostream>
#include <memory>

using namespace Game;

const std::string Handler::video = "data/ini/video.ini";
const std::string Handler::speech = "data/ini/speech.ini";

const std::map<std::string, Handler::Section> Handler::sections = {
	{"Speech",Handler::SPEECH},
	{"Video",Handler::VIDEO}
};

const std::map<std::string, Handler::ParametersVideo> Handler::parameters_video = {
	{ "Filename", Handler::FILENAME },
	{ "Video", Handler::COMMENT }
};


void Handler::Initialize()
{
	BigStream videoIni;
	videoIni.open(video);
	std::string videoStr = videoIni.readAll();
	std::string testStr = "line\r\nline2\r\nline3";
	ParseINI(videoStr);
}

inline bool GetLine(std::string& str,std::istream& in = std::cin, char delim = '\n') 
{
	if (in.eof() || (!in.good())) { return false; }
	std::getline(in, str, delim);
	return !in.fail() && (in.good() || in.eof());
}

bool Handler::ParseINI(std::string& content)
{
	std::istringstream iss(content);
	std::string line;
	ParserState state = NEW_SECTION;
	int pos = 0;
	int n = 1;
	std::shared_ptr<GameData::INIObject> obj = nullptr;
	std::string sectionName;
	//make unix line endings:
	
	while (std::getline(iss, line))
	{
		line = line.substr(0, line.find('/'));
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
		if (line.size() == 0)
			continue;

		switch (state)
		{
		case Game::Handler::NEW_SECTION:
			{
				pos = line.find(' ');
				std::string sectionStr = line.substr(0, pos);
				auto it = sections.find(sectionStr);

				if (it == sections.end())
				{
					std::cout << "Unknown section in line: " << n << std::endl;
				}
				else 
				{
					auto sectionType = it->second;
					switch (sectionType)
					{
					case VIDEO:
						{
							state = PARSE_VIDEO;
							obj = std::make_shared<GameData::Video>();
							sectionName = line.substr(pos, line.size());
						}
						break;
					}
				}
			}
			break;
		case Game::Handler::PARSE_VIDEO:
			{
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				pos = line.find('=');
				std::string paramStr = line.substr(0, pos);
				auto it = parameters_video.find(paramStr);
				if (it == parameters_video.end())
				{
					std::cout << "Unknown parameter \""<<paramStr<< "\" in line: " << n << std::endl;
				}
				else
				{
					auto paramType = it->second;
					switch (paramType)
					{
					case FILENAME:
					{
								  state = PARSE_VIDEO;
								  auto video = std::dynamic_pointer_cast<GameData::Video>(obj);
								  video->filename = line.substr(pos, line.size());
					}
						break;
					}
				}
			}
			break;
		default:
			break;
		}
		++n;
	}

	return true;
}