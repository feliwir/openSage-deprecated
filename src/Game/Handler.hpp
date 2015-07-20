// Stephan Vedder 2015
#pragma once
#include <string>
#include <vector> 
#include <chrono>
#include "../Loaders/Mp3Stream.hpp"
#include "../Graphics/Video.hpp"
//#include "../Loaders/AptFile.hpp"
#include <SFML/Window.hpp>


namespace Loaders
{
    class AptFile{};
}

namespace Game
{
	class Handler
	{
	private:
		enum GameState
		{
			CINEMATIC = 0,
			APT_FILE = 1,
			LOADING_SCREEN = 2,
		};

		struct StateArgs
		{
			inline GameState& getType()
			{
				return m_state;
			}

			virtual ~StateArgs()
			{

			}

		protected:
			GameState m_state;
		};

		struct StateInfo
		{
			virtual bool isDone() = 0;
			inline GameState getType()
			{
				return state_type;
			}

		protected:
			GameState state_type;
		};

		struct CinematicInfo : public StateInfo
		{
			inline bool isDone()
			{
				if (mp3->getStatus() == Loaders::Mp3Stream::Stopped && vid->getStatus() == Loaders::Vp6Stream::Stopped)
				{
					return true;
				}

				return false;
			};

			CinematicInfo(std::shared_ptr<Graphics::Video> video, std::shared_ptr<Loaders::Mp3Stream> audio, bool skippable = false);

            inline std::shared_ptr<Graphics::Video> getVid()
			{
				return vid;
			}

			inline std::shared_ptr<Loaders::Mp3Stream> getMp3()
			{
				return mp3;
			}

			inline void skip()
			{
				if (canSkip)
				{
					vid->stop();
					mp3->stop();
				}

			}

		private:
			std::shared_ptr<Graphics::Video> vid;
			std::shared_ptr<Loaders::Mp3Stream> mp3;
			bool canSkip;
		};

		struct LoadingScreenInfo : public StateInfo
		{
			inline bool isDone()
			{
				if ((std::chrono::high_resolution_clock::now() - start) > std::chrono::seconds(1))
					return true;
				else
					return false;
			};

			LoadingScreenInfo(std::shared_ptr<Loaders::Vp6Stream> video, std::shared_ptr<Graphics::Texture> tex);

			inline std::shared_ptr<Loaders::Vp6Stream> getVP6()
			{
				return vp6;
			}

		private:
			std::shared_ptr<Loaders::Vp6Stream> vp6;
			std::shared_ptr<Graphics::Texture> tex;
			std::chrono::high_resolution_clock::time_point start;
		};

		/*struct AptInfo : public StateInfo
		{
			inline bool isDone()
			{
				return false;
			};

			AptInfo(std::shared_ptr<Loaders::AptFile> apt);

			inline std::shared_ptr<Loaders::AptFile> getApt()
			{
				return apt;
			}
		private:
			std::shared_ptr<Loaders::AptFile> apt;
		};*/

		struct CinematicArgs : public StateArgs
		{
			CinematicArgs(bool skipable)
			{
				m_state = CINEMATIC;
				canSkip = skipable;
				loop = false;
			}

			inline bool isSkipable()
			{
				return canSkip;
			}
		protected:
			bool canSkip;
			bool loop;
		};

		struct LoadingScreenArgs : public StateArgs
		{
			LoadingScreenArgs(std::string name)
			{
				m_state = LOADING_SCREEN;
				imgName = name;
			}

			inline std::string getImageName()
			{
				return imgName;
			}
		protected:
			std::string imgName;
		};

		struct AptArgs : public StateArgs
		{
			AptArgs(std::string name)
			{
				m_state = APT_FILE;
				aptName = name;
			}

			inline const std::string getAptName()
			{
				return aptName;
			}
		protected:
			std::string aptName;
		};

		struct LoadInfo
		{
		public:
			LoadInfo(std::string name, StateArgs* args)
			{
				this->name = name;
				this->args = args;
				this->loaded = false;
			}

		public:
			std::string name;
			StateArgs* args;
			bool loaded;
		};

		static std::vector<LoadInfo> loadOrder;

		static void getState();
	public:
		static void initialize();
		static void parseGameInis();
        static void update(sf::Window& window);
		static void keyDown(const sf::Event::KeyEvent& keyEv);
		static void keyUp(const sf::Event::KeyEvent& keyEv);
	private:

		static std::shared_ptr<StateInfo> cState;
		static int state_index;

		static bool escPressed;
	};
}
