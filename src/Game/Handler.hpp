#pragma once
#include <string>

namespace Game
{
	class Handler
	{
	public:
		static void Initialize();
	private:
		//INI paths
		static const std::string video;
		static const std::string speech;
		static bool ParseINI(const std::string& content);
	};
}
