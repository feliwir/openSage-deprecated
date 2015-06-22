#pragma once
#include <map>
#include <string>
#include <stdint.h>
#include <iostream>

class GameData
{
public:
	struct INIObject
	{
		inline virtual void PrintInfo()
		{
			std::cout << "No info available" << std::endl;
		}
	};

	struct Speech : INIObject
	{
		std::string filename;
		uint8_t mixer;
		uint8_t volume;

		inline virtual void PrintInfo()
		{
			std::cout << "Speech: Filename: " << filename << std::endl;
		}
	};

	struct Video : INIObject
	{
		std::string filename;

		inline virtual void PrintInfo()
		{
			std::cout << "Video: Filename: " << filename << std::endl;
		}

	};
public:
	inline void AddSpeech(std::string& name,Speech& speech)
	{
		m_speech[name] = speech;
	}

	inline void AddVideo(std::string& name, Video& video)
	{
		m_videos[name] = video;
	}
private:
	std::map<std::string, Speech> m_speech;
	std::map<std::string, Video> m_videos;
};