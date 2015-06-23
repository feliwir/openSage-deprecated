#include "Handler.hpp"
#include "GameData.hpp"
#include "INI.hpp"
#include "../Loaders/BigStream.hpp"
#include <memory>

using namespace Game;

const std::string Handler::video = "data/ini/video.ini";
const std::string Handler::speech = "data/ini/speech.ini";

const std::map<std::string, Handler::GameState> Handler::loadOrder = {
	{"EALogo",Handler::CINEMATIC},
};

void Handler::Initialize()
{
	BigStream videoIni,speechIni;
	videoIni.open(video);
	std::string videoStr = videoIni.readAll();
	INI::Parse(videoStr);
	speechIni.open(speech);
	std::string speechStr = speechIni.readAll();
	INI::Parse(speechStr);
}


void Handler::Update()
{


}