#include "Audio.hpp"
#include <mpg123.h>
#include <iostream>

std::vector<std::shared_ptr<Loaders::Mp3Stream>> Audio::m_streams;
std::thread Audio::m_thread;
bool Audio::m_running = true;

void Audio::Initialize()
{
	int  err = MPG123_OK;
	if ((err = mpg123_init()) != MPG123_OK)
	{
		std::cerr << mpg123_plain_strerror(err) << std::endl;
	}

	m_running = true;
	m_thread = std::thread(Update);
}

void Audio::Release()
{
	m_running = false;
	//wait until this thread terminates
	m_thread.join();
	mpg123_exit();
}

void Audio::RegisterStream(Loaders::Mp3Stream& stream)
{
	m_streams.push_back(std::shared_ptr<Loaders::Mp3Stream>(&stream));
}

void Audio::Update()
{
	while (m_running)
	{
		for each (auto stream in m_streams)
		{
			stream->update();
		}

		//no need to check for updates this frequently
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}