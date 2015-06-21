#include "Handler.hpp"
#include "GameData.hpp"
#include "../Loaders/BigStream.hpp"
#include <sstream>

using namespace Game;

const std::string Handler::video = "data\\ini\\video.ini";
const std::string Handler::speech = "data\\ini\\speech.ini";

void Handler::Initialize()
{
	BigStream videoIni;
	videoIni.open(video);
	auto videoStr = videoIni.readAll();
	ParseINI(videoStr);
}

bool Handler::ParseINI(const std::string& content)
{

}