#pragma once
#include <map>
#include <string>
#include <stdint.h>

class GameData
{
public:
	struct Speech
	{
		const std::string filename;
		uint8_t mixer;
		uint8_t volume;
	};

	struct Video
	{
		const std::string filename;
	};
public:
	inline void SetSpeech(std::map<std::string,Speech>& speech)
	{
		m_speech = speech;
	}

	inline void SetVideo(std::map<std::string, Video>& videos)
	{
		m_videos = videos;
	}
private:
	std::map<std::string, Speech> m_speech;
	std::map<std::string, Video> m_videos;
};