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
			PARSE_AMBIENT_STREAM,
			PARSE_VIDEO,
			PARSE_DIALOG_EVENT,
			PARSE_LANGUAGE,
		};

		enum Section
		{
			AMBIENT_STREAM         = 0,
			DIALOG_EVENT,
			VIDEO,
			LANGUAGE
		};

		enum Parameters
		{
			COMMENT                = 0,
			END,
			FILENAME,
			LOCALFONTFILE,
			TYPE,
			VOLUME,
			SUBMIX_SLIDER,
		};

		static const std::map<std::string, Section> sections;
		static const std::map<std::string, Parameters> parameters;
	public:
		static bool Parse(const std::string& content);
	};
}