#pragma once
#include <map>
#include <string>
#include <stdint.h>
#include <iostream>
#include <memory>

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

	struct DialogEvent : INIObject
	{
		std::string filename;
		std::string comment;
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
		std::string comment;

		inline virtual void PrintInfo()
		{
			std::cout << "Video: Filename: " << filename << std::endl;
		}

	};
public:
	static inline void AddDialogEvent(std::string& name,std::shared_ptr<DialogEvent> speech)
	{
		gd_dialogs[name] = speech;
	}

	static inline void AddVideo(std::string& name, std::shared_ptr<Video> video)
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
private:
	static std::map<std::string, std::shared_ptr<DialogEvent>> gd_dialogs;
	static std::map<std::string, std::shared_ptr<Video>> gd_videos;
};