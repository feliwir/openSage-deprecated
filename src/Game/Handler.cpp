// Stephan Vedder 2015
#include "Handler.hpp"
#include "GameData.hpp"
#include "INI.hpp"
#include "../Loaders/BigStream.hpp"
#include "../Loaders/Util.hpp"
#include "../FileSystem.hpp"
#include "../GraphicsSystem.hpp"
#include <memory>


using namespace Loaders;
using namespace Game;

//TODO: change this for different gametitles
//loading order of the game is hardcoded
std::vector<Handler::LoadInfo> Handler::loadOrder = {
	Handler::LoadInfo("EALogoMovie", new Handler::CinematicArgs(false)),
	Handler::LoadInfo("NewLineLogo", new Handler::CinematicArgs(false)),
	Handler::LoadInfo("TolkienLogo", new Handler::CinematicArgs(false)),
	Handler::LoadInfo("Overall_Game_Intro", new Handler::CinematicArgs(true)),
	Handler::LoadInfo("LoadingRing", new Handler::LoadingScreenArgs("titlescreenuserinterface.jpg")),
	//Handler::LoadInfo("MainMenu", new Handler::AptArgs(""))
};

std::shared_ptr<Handler::StateInfo> Handler::cState;
int Handler::state_index = 0;
bool Handler::escPressed = false;

Handler::CinematicInfo::CinematicInfo(std::shared_ptr<Graphics::Video> video, std::shared_ptr<Loaders::Mp3Stream> audio,bool skipable)
{
	vid = video;
	mp3 = audio;
	canSkip = skipable;
	state_type = CINEMATIC;
}

Handler::LoadingScreenInfo::LoadingScreenInfo(std::shared_ptr<Loaders::Vp6Stream> video, std::shared_ptr<Graphics::Texture> texture)
{
	vp6 = video;
	tex = texture;
	start = std::chrono::high_resolution_clock::now();
	state_type = LOADING_SCREEN;
}

void Handler::Initialize()
{
    //parse a couple ini's on startup
    auto iniStream = FileSystem::Open(GameData::videoINI);
	std::string iniStr;
    iniStr = Util::ReadAll(iniStream);
	INI::Parse(iniStr);
    iniStream = FileSystem::Open(GameData::speechINI);
    iniStr = Util::ReadAll(iniStream);
	INI::Parse(iniStr);
    iniStream = FileSystem::Open(GameData::languageINI);
    iniStr = Util::ReadAll(iniStream);
	INI::Parse(iniStr);
	GetState();	
}

//go into the next gamestate
void Handler::GetState()
{
	bool done = false;

	if (loadOrder.back().loaded == true)
	{
		std::cout << "Finished game" << std::endl;
	}
	

    //check which type of gamestate does follow now
	for (auto& l : loadOrder)
	{
		if (l.loaded)
			continue;
	
		l.loaded = true;

		switch (l.args->GetType())
		{
		case CINEMATIC:
		{			
			auto args =	dynamic_cast<CinematicArgs*>(l.args);
			auto video = GameData::GetVideo(l.name);
			if (video == nullptr)
				continue;

            std::shared_ptr<Graphics::Video> vid = std::make_shared<Graphics::Video>();
			if (!vid->Create(GameData::videoDIR + video->filename + ".vp6",glm::vec2(0,0),glm::vec2(1024,768)))
				continue;

			vid->play();

			std::shared_ptr<Loaders::Mp3Stream> mp3 = std::make_shared<Loaders::Mp3Stream>();
			auto dialog = GameData::GetDialogEvent(video->filename);
			if (dialog != nullptr)
			{							
				std::transform(dialog->filename.begin(), dialog->filename.end(), dialog->filename.begin(), ::tolower);

				if (mp3->open(GameData::speechDIR + dialog->filename))
				{
					mp3->play();
				}
			}
			done = true;
            GraphicsSystem::AddVideo(vid);
			cState = std::make_shared<CinematicInfo>(vid, mp3, args->IsSkipable());
			
		}
			break;
		case APT_FILE:
		{
			auto args = dynamic_cast<AptArgs*>(l.args);

			std::shared_ptr<Loaders::AptFile> apt = std::make_shared<Loaders::AptFile>();
            auto aptStream = FileSystem::Open(l.name + ".apt");

            auto constStream = FileSystem::Open(l.name + ".const");

			//apt->loadFromStream(aptStream, constStream,l.name);
			done = true;

			/*cState = std::make_shared<AptInfo>(apt);*/
		}
			break;
		case LOADING_SCREEN:
		{
			auto args = dynamic_cast<LoadingScreenArgs*>(l.args);
			auto video = GameData::GetVideo(l.name);

			if (video == nullptr)
				continue;

			std::shared_ptr<Loaders::Vp6Stream> vp6 = std::make_shared<Loaders::Vp6Stream>();

			if (vp6->open(GameData::videoDIR + video->filename + ".vp6"))
			{
				vp6->play();
			}

			std::string path = GameData::compiledtexDIR + args->GetImageName().substr(0, 2) + "/" + args->GetImageName();
            auto imgStream = FileSystem::Open(path);

            std::shared_ptr<Graphics::Texture> tex = std::make_shared<Graphics::Texture>();
            tex->loadFromStream(*imgStream);

			done = true;

			cState = std::make_shared<LoadingScreenInfo>(vp6, tex);
		}
			break;
		default:
			break;
		}

		if (done)
			break;
	}
}


//check if the current gamestate is done
//when it's the case go to the next gamestate
void Handler::Update(sf::Window& window)
{
	if (!cState->IsDone())
	{
		switch (cState->GetType())
		{
		case CINEMATIC:
		{
			auto cinematic = std::static_pointer_cast<CinematicInfo>(cState);
	
			if (escPressed)
			{
				escPressed = false;
				cinematic->Skip();
			}
		}
			break;
		case LOADING_SCREEN:
		{                          
			auto loading_screen = std::static_pointer_cast<LoadingScreenInfo>(cState);

		}
			break;
		case APT_FILE:
		{
			/*auto apt_file = std::static_pointer_cast<AptInfo>(cState);
			auto apt = apt_file->GetApt();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));*/
			//apt->Update();
		}
			break;
		}
	}
	else
	{
		GetState();
	}
}

void Handler::KeyDown(const sf::Event::KeyEvent& keyEv)
{
	switch (keyEv.code)
	{
	case sf::Keyboard::Escape:
		escPressed = true;
		break;
	default:
		break;
	}
}

void Handler::KeyUp(const sf::Event::KeyEvent& keyEv)
{
	switch (keyEv.code)
	{
	case sf::Keyboard::Escape:
		escPressed = false;
		break;
	default:
		break;
	}
}