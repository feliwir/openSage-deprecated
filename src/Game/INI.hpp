#pragma once
#include <map>
#include <string>

namespace Game
{
class INI
{
	private:
		enum ParserState
		{
			NEW_SECTION 		= 0,
			PARSE_VIDEO 		= 1,
			PARSE_DIALOG_EVENT	= 2,
		};

		enum Section
		{
			VIDEO 			= 0,
			DIALOG_EVENT 	= 1
		};

		enum Parameters
		{
			END 			= 0,
			FILENAME 		= 1,
			COMMENT 		= 2,
			VOLUME 			= 3,
			SUBMIX_SLIDER	= 4,
		};

		static const std::map<std::string, Section> sections;
		static const std::map<std::string, Parameters> parameters;
	public:
		static bool Parse(const std::string& content);
}; 
}