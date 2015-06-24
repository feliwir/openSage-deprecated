#pragma once
#include <string>
#include <vector> 
#include "../Loaders/Mp3Stream.hpp"
#include "../Loaders/Vp6Stream.hpp"

namespace Game
{
	class Handler
	{
	private:
		enum GameState
		{
			CINEMATIC		= 0,
			APT				= 1,
			LOADING_SCREEN	= 2,
		};

		struct StateArgs
		{
			inline GameState& GetType()
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
			virtual bool IsDone() = 0;
			inline GameState GetType()
			{
				return state_type;
			}
		protected:
			GameState state_type;
		};

		struct CinematicInfo : public StateInfo
		{
			inline bool IsDone()
			{
				if (mp3->getStatus() == Loaders::Mp3Stream::Stopped && vp6->getStatus() == Loaders::Vp6Stream::Stopped)
				{
					return true;
				}
					
				return false;
			};

			CinematicInfo(std::shared_ptr<Loaders::Vp6Stream> video, std::shared_ptr<Loaders::Mp3Stream> audio,bool skippable = false);

			inline std::shared_ptr<Loaders::Vp6Stream> GetVP6()
			{
				return vp6;
			}

			inline std::shared_ptr<Loaders::Mp3Stream> GetMP3()
			{
				return mp3;
			}

			inline void Skip()
			{
				if (canSkip)
				{
					vp6->stop();
					mp3->stop();
				}
					
			}
		private:
			std::shared_ptr<Loaders::Vp6Stream> vp6;
			std::shared_ptr<Loaders::Mp3Stream> mp3;
			bool canSkip;
		};

		struct LoadingScreenInfo : public StateInfo
		{
			inline bool IsDone()
			{
				return false;
			};

			LoadingScreenInfo(std::shared_ptr<Loaders::Vp6Stream> video, std::shared_ptr<sf::Texture> tex);

			inline std::shared_ptr<Loaders::Vp6Stream> GetVP6()
			{
				return vp6;
			}

			inline std::shared_ptr<sf::Texture> GetTex()
			{
				return tex;
			}
		private:
			std::shared_ptr<Loaders::Vp6Stream> vp6;
			std::shared_ptr<sf::Texture> tex;
		};
		
		struct CinematicArgs : public StateArgs
		{
			CinematicArgs(bool skipable)
			{
				m_state = CINEMATIC;
				canSkip = skipable;
				loop = false;
			}

			inline bool IsSkipable()
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

			inline std::string GetImageName()
			{
				return imgName;
			}
		protected:
			std::string imgName;
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

		static void GetState();
	public:
		static void Initialize();
		static void Update(sf::RenderWindow& window);
		static void KeyDown(sf::Event::KeyEvent& keyEv);
		static void KeyUp(sf::Event::KeyEvent& keyEv);
	private:
		//INI paths
		static const std::string videoINI;
		static const std::string speechINI;
		//DATA paths
		static const std::string videoDIR;
		static const std::string speechDIR;
		static const std::string compiledtexDIR;
		static std::shared_ptr<StateInfo> cState;
		static int state_index;

		static bool escPressed;
	};
}
