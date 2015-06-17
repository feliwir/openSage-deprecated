#pragma once
#include <vector>
#include <thread>
#include "Loaders/Mp3Stream.hpp"

class Audio
{
public:
	static void Initialize();
	static void Release();

	static void RegisterStream(Loaders::Mp3Stream& stream);
	static inline std::thread& GetThread()
	{
		return m_thread;
	}
private:
	static void Update();

	static bool	m_running;
	static std::vector<std::shared_ptr<Loaders::Mp3Stream>> m_streams;
	static std::thread m_thread;
};