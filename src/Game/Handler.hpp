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
		struct StateInfo
		{
			virtual bool IsDone() = 0;
		};

		struct CinematicInfo : public StateInfo
		{
			inline bool IsDone()
			{
				if(mp3.getStatus()==Loaders::Mp3Stream::Stopped &&
					vp6.getStatus()==Loaders::Vp6Stream::Stopped)
					return true;

				return false;
			};

		private:
			Loaders::Vp6Stream vp6;
			Loaders::Mp3Stream mp3;
		};

		enum GameState
		{
			CINEMATIC 	= 0,
			APT			= 1,
		};


		struct LoadInfo
		{
			const std::string name;
			GameState state;
		};

		static const std::vector<LoadInfo> loadOrder;
	public:
		static void Initialize();
		static void Update();
	private:
		//INI paths
		static const std::string videoINI;
		static const std::string speechINI;
		static std::shared_ptr<StateInfo> cState;
		static int state_index;
	};
}
