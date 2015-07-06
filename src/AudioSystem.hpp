// (C) Stephan Vedder 2015
#pragma once
#include <vector>
#include <thread>
#include "Loaders/Mp3Stream.hpp"

class AudioSystem
{
public:
	static void Initialize();
	static void Release();

	static void RegisterStream(Loaders::Mp3Stream *stream);
	static void UnregisterStream(Loaders::Mp3Stream* stream);
	static inline std::thread& GetThread()
	{
		return m_thread;
	}
private:
	static void Update();

	static bool	m_running;
	static std::vector<Loaders::Mp3Stream*> m_streams;
	static std::thread m_thread;
};