// Stephan Vedder 2015
#include "AudioSystem.hpp"
#include <mpg123.h>
#include <iostream>

std::vector<Loaders::Mp3Stream*> AudioSystem::m_streams;
std::thread AudioSystem::m_thread;
bool AudioSystem::m_running = false;

void AudioSystem::Initialize()
{
	int  err = MPG123_OK;
	if ((err = mpg123_init()) != MPG123_OK)
	{
		std::cerr << mpg123_plain_strerror(err) << std::endl;
	}

	m_running = true;
	m_thread = std::thread(Update);
}

void AudioSystem::Release()
{
	m_running = false;
	//wait until this thread terminates
	m_thread.join();
	mpg123_exit();
}

void AudioSystem::RegisterStream(Loaders::Mp3Stream* stream)
{
	m_streams.push_back(stream);
}
void AudioSystem::UnregisterStream(Loaders::Mp3Stream* stream)
{
	m_streams.erase(std::remove(m_streams.begin(), m_streams.end(), stream), m_streams.end());
}

void AudioSystem::Update()
{
	while (m_running)
	{
        //update each audio stream
		for(auto& stream : m_streams)
		{
			stream->update();
		}

		//no need to check for updates this frequently
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (m_streams.size() > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}