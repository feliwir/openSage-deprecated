// Stephan Vedder 2015
#include "INI.hpp"
#include "GameData.hpp"
#include <memory>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace Game;

//all tokens that show a new section
//keep them in alphabetical order
const std::map<std::string, INI::Section> INI::sections = {
	{ "AmbientStream", INI::AMBIENT_STREAM},
	{ "DialogEvent",INI::DIALOG_EVENT},
	{ "Language", INI::LANGUAGE},
	{ "Video", INI::VIDEO },
};

//all tokens that are a parameter of a section
//keep them in alphabetical order
const std::map<std::string, INI::Parameters> INI::parameters = {
	{ "Comment", INI::COMMENT },
	{ "END", INI::END },
	{ "End", INI::END },
	{ "Filename", INI::FILENAME },
	{ "Type", INI::TYPE },
	{ "Volume", INI::VOLUME },
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
        //end the line after any ; or //
		line = line.substr(0, line.find(';'));
		line = line.substr(0, line.find("//"));

        //remove tabs and windows newline 
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
		if (line.size() == 0)
			continue;

        //check the current parser state
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
				case AMBIENT_STREAM:
				{
					state = PARSE_AMBIENT_STREAM;
					obj = std::make_shared<GameData::AmbientStream>();
					sectionName = line.substr(pos, line.size());
					sectionName.erase(std::remove(sectionName.begin(),sectionName.end(), ' '), sectionName.end());
				}
				break;
				case DIALOG_EVENT:
				{
					state = PARSE_DIALOG_EVENT;
					obj = std::make_shared<GameData::DialogEvent>();
					sectionName = line.substr(pos, line.size());
					sectionName.erase(std::remove(sectionName.begin(),sectionName.end(), ' '), sectionName.end());
				}
				break;
				case VIDEO:
				{
					state = PARSE_VIDEO;
					obj = std::make_shared<GameData::Video>();
					sectionName = line.substr(pos, line.size());
					sectionName.erase(std::remove(sectionName.begin(), sectionName.end(), ' '), sectionName.end());
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
		case PARSE_AMBIENT_STREAM:
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
					GameData::AddAmbientStream(sectionName, std::static_pointer_cast<GameData::AmbientStream>(obj));
				}
				break;
				case FILENAME:
				{
					auto aStream = std::static_pointer_cast<GameData::AmbientStream>(obj);
					aStream->filename = line.substr(pos + 1, line.size());
				}
				break;
				case VOLUME:
				{
					auto aStream = std::static_pointer_cast<GameData::AmbientStream>(obj);
					aStream->volume = line.substr(pos + 1, line.size());
				}
				break;
				case TYPE:
				{
					auto aStream = std::static_pointer_cast<GameData::AmbientStream>(obj);
					aStream->type = line.substr(pos + 1, line.size());
				}
				break;
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
						video->filename = line.substr(pos + 1, line.size());
					}
					break;
					case COMMENT:
					{
						auto video = std::static_pointer_cast<GameData::DialogEvent>(obj);
						video->comment = line.substr(pos + 1, line.size());
					}
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