// Stephan Vedder 2015
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
			NEW_SECTION		       = 0,
			PARSE_AMBIENT_STREAM   = 1,
			PARSE_VIDEO            = 2,
			PARSE_DIALOG_EVENT     = 3,
			PARSE_LANGUAGE         = 4
		};

		enum Section
		{
			AMBIENT_STREAM         = 0,
			DIALOG_EVENT           = 1,
			VIDEO                  = 2,
			LANGUAGE               = 3
		};

		enum Parameters
		{
			COMMENT                = 0,
			END                    = 1,
			FILENAME               = 2,
			LOCALFONTFILE          = 3,
			TYPE                   = 4,
			VOLUME                 = 5,
			SUBMIX_SLIDER          = 6
		};

		static const std::map<std::string, Section> sections;
		static const std::map<std::string, Parameters> parameters;
	public:
		static bool parse(const std::string& content);
	};
}