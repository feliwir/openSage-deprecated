#include "Handler.hpp"
#include "GameData.hpp"
#include "INI.hpp"
#include "../Loaders/BigStream.hpp"
#include <memory>

using namespace Game;

const std::string Handler::videoINI = "data/ini/video.ini";
const std::string Handler::speechINI = "data/ini/speech.ini";

const std::vector<Handler::LoadInfo> Handler::loadOrder = {
	Handler::LoadInfo{"EALogo",Handler::CINEMATIC},
};

std::shared_ptr<Handler::StateInfo> Handler::cState;
int Handler::state_index = 0;

void Handler::Initialize()
{
	BigStream videoStream,speechStream;
	videoStream.open(videoINI);
	std::string videoStr = videoStream.readAll();
	INI::Parse(videoStr);
	speechStream.open(speechINI);
	std::string speechStr = speechStream.readAll();
	INI::Parse(speechStr);

	for(const auto& l : loadOrder)
	{
		switch(l.state)
		{
			case CINEMATIC:
			{
					std::cout << "Loading CINEMATIC" << std::endl;
					auto video = GameData::GetVideo(l.name);
					video->PrintInfo();
					auto dialog = GameData::GetDialogEvent(l.name);
					dialog->PrintInfo();
					cState = std::make_shared<CinematicInfo>();

			}
			break;
			default:
			break;
		}
	}
}


void Handler::Update()
{


}