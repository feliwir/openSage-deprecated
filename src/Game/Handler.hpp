#pragma once
#include <string>
#include <map>

namespace Game
{
	class Handler
	{
	private:
		enum ParserState
		{
			NEW_SECTION = 0,
			PARSE_VIDEO = 1,
		};

		enum Section
		{
			VIDEO = 0,
			SPEECH = 1
		};

		enum ParametersVideo
		{
			FILENAME = 0,
			COMMENT = 1,
		};

		static const std::map<std::string, Section> sections;
		static const std::map<std::string, ParametersVideo> parameters_video;
	public:
		static void Initialize();
	private:
		//INI paths
		static const std::string video;
		static const std::string speech;
		static bool ParseINI(std::string& content);
	};
}
