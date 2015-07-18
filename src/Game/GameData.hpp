// Stephan Vedder 2015
#pragma once
#include <map>
#include <string>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <vector>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System.hpp>
#include "../FileSystem.hpp"

class GameData
{
public:
	struct INIObject
	{};

	struct DialogEvent : INIObject
	{
		std::string filename;
		std::string comment;
		uint8_t mixer;
		uint8_t volume;
	};

	struct Video : INIObject
	{
		std::string filename;
		std::string comment;
	};

	struct Language : INIObject
	{
		std::vector<std::string> fonts;
	};
public:
	static inline void AddDialogEvent(const std::string& name, const std::shared_ptr<DialogEvent> speech)
	{
		gd_dialogs[name] = speech;
	}

	static inline void AddVideo(const std::string& name, const std::shared_ptr<Video> video)
	{
		gd_videos[name] = video;
	}

	static inline std::shared_ptr<DialogEvent>& GetDialogEvent(const std::string& name)
	{
		return gd_dialogs[name];
	}

	static inline std::shared_ptr<Video>& GetVideo(const std::string& name)
	{
		return gd_videos[name];
	}

	static inline void SetLanguage(const std::shared_ptr<Language> lang)
	{
		gd_language = lang;

		for (auto& name : lang->fonts)
		{
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            auto stream = FileSystem::Open(name);

			/*sf::Font font;
			if (!font.loadFromStream(*stream))
				continue;*/

			if (gd_defaultFont == nullptr)
				gd_defaultFont = stream;

			gd_fonts["bla"] = stream;
		}
	}

	static inline std::shared_ptr<sf::InputStream> GetFont(const std::string& name)
	{
		auto font = gd_fonts[name];
		if (font == nullptr)
			font = gd_defaultFont;

		return font;
	}

public:
	//INI paths
	static const std::string videoINI;
	static const std::string speechINI;
	static const std::string languageINI;
	//DATA paths
	static const std::string dataDIR;
	static const std::string artDIR;
	static const std::string videoDIR;
	static const std::string speechDIR;
	static const std::string compiledtexDIR;

private:
	static std::map<std::string, std::shared_ptr<DialogEvent>> gd_dialogs;
	static std::map<std::string, std::shared_ptr<Video>> gd_videos;
	static std::shared_ptr<Language> gd_language;
	static std::map<std::string, std::shared_ptr<sf::InputStream>> gd_fonts;

    static std::shared_ptr<sf::InputStream> gd_defaultFont;
};