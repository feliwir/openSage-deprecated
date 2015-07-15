// Stephan Vedder 2015
#include "INI.hpp"
#include "GameData.hpp"
#include <memory>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace Game;

const std::map<std::string, INI::Section> INI::sections = {
	{"DialogEvent",INI::DIALOG_EVENT},
	{"Video",INI::VIDEO},
	{"Language", INI::LANGUAGE}
};

const std::map<std::string, INI::Parameters> INI::parameters = {
	{ "END", INI::END },
	{ "End", INI::END },
	{ "Filename", INI::FILENAME },
	{ "Comment", INI::COMMENT },
	{ "LocalFontFile", INI::LOCALFONTFILE }
};

bool INI::Parse(const std::string& content)
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
		line = line.substr(0, line.find(';'));
		line = line.substr(0, line.find("//"));
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
		if (line.size() == 0)
			continue;

		switch (state)
		{
		case NEW_SECTION:
			{
				pos = line.find(' ');
				std::string sectionStr = line.substr(0, pos);
				sectionStr.erase(std::remove(sectionStr.begin(), sectionStr.end(), ' '), sectionStr.end());
				auto it = sections.find(sectionStr);

				if (it != sections.end())
				{
					auto sectionType = it->second;
					switch (sectionType)
					{
					case VIDEO:
					{
						state = PARSE_VIDEO;
						obj = std::make_shared<GameData::Video>();
						sectionName = line.substr(pos, line.size());
						sectionName.erase(std::remove(sectionName.begin(), 
							sectionName.end(), ' '), sectionName.end());
					}
					break;
					case DIALOG_EVENT:
					{
						state = PARSE_DIALOG_EVENT;
						obj = std::make_shared<GameData::DialogEvent>();
						sectionName = line.substr(pos, line.size());
						sectionName.erase(std::remove(sectionName.begin(),
							sectionName.end(), ' '), sectionName.end());
					}
					break;
					case LANGUAGE:
					{
						state = PARSE_LANGUAGE;
						obj = std::make_shared<GameData::Language>();
					}
					break;
					}
				}
			}
			break;
		case PARSE_VIDEO:
			{
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				pos = line.find('=');
				std::string paramStr = line.substr(0, pos);
				auto it = parameters.find(paramStr);
				if (it != parameters.end())
				{
					auto paramType = it->second;
					switch (paramType)
					{
						case END:
						{
							state = NEW_SECTION;
							GameData::AddVideo(sectionName, std::static_pointer_cast<GameData::Video>(obj));
						}
						break;
						case FILENAME:
						{
							auto video = std::static_pointer_cast<GameData::Video>(obj);
							video->filename = line.substr(pos+1, line.size());
						}
						break;
						case COMMENT:
						{
							auto video = std::static_pointer_cast<GameData::Video>(obj);
							video->comment = line.substr(pos+1, line.size());
						}
					}
				}
			}
			break;
		case PARSE_DIALOG_EVENT:
			{
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				pos = line.find('=');
				std::string paramStr = line.substr(0, pos);
				auto it = parameters.find(paramStr);

				if (it != parameters.end())
				{
					auto paramType = it->second;
					switch (paramType)
					{
						case END:
						{
							state = NEW_SECTION;
							GameData::AddDialogEvent(sectionName, std::static_pointer_cast<GameData::DialogEvent>(obj));
						}
						break;
						case FILENAME:
						{
							auto video = std::static_pointer_cast<GameData::DialogEvent>(obj);
							video->filename = line.substr(pos+1, line.size());
						}
						break;
						case COMMENT:
						{
							auto video = std::static_pointer_cast<GameData::DialogEvent>(obj);
							video->comment = line.substr(pos+1, line.size());
						}
					}
				}
			}
			break;
		case PARSE_LANGUAGE:
		{
			line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
			pos = line.find('=');
			std::string paramStr = line.substr(0, pos);
			auto it = parameters.find(paramStr);

			if (it != parameters.end())
			{
				auto paramType = it->second;
				switch (paramType)
				{
				case END:
				{
					state = NEW_SECTION;
					GameData::SetLanguage(std::static_pointer_cast<GameData::Language>(obj));
				}
					break;
				case LOCALFONTFILE:
				{
					auto lang = std::static_pointer_cast<GameData::Language>(obj);
					lang->fonts.push_back(line.substr(pos + 1, line.size()));
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