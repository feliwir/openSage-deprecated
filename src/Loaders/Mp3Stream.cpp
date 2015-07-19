// Stephan Vedder 2015
#include "Mp3Stream.hpp"
#include "../AudioSystem.hpp"
#include <iostream>
#include <mpg123.h>

using namespace Loaders;


Mp3Stream::Mp3Stream() : m_handle(nullptr), m_isDone(false)
{

}

Mp3Stream::~Mp3Stream()
{
	stop();

	mpg123_close(m_handle);
	mpg123_delete(m_handle);

}

bool Mp3Stream::open(const std::string& name)
{
	stop();
	size_t size = 0;
	int ret = MPG123_NEED_MORE;
	int err = MPG123_OK;
	ssize_t len = 0;
	m_handle = mpg123_new(nullptr, &err);
	long rate;
	int channels, enc;
    m_stream = FileSystem::Open(name);
	if (!m_stream)
		return false;

	if (!m_handle)
	{
		std::cerr << "Unable to create mpg123 handle: " << mpg123_plain_strerror(err) << std::endl;
		return false;
	}

	if (mpg123_open_feed(m_handle)!= MPG123_OK)
	{
		std::cerr << mpg123_strerror(m_handle) << std::endl;
		return false;
	}

	len = m_stream->read(m_buf, INBUFF);

    //read until we found out the format of this mp3 file
	while (ret==MPG123_NEED_MORE)
	{
		ret = mpg123_decode(m_handle, m_buf, len, m_out, OUTBUFF, &size);

		if (ret == MPG123_NEW_FORMAT)
		{
			mpg123_getformat(m_handle, &rate, &channels, &enc);
			std::cout << "New format: " << rate << " Hz, " << channels << " channels, encoding value " << enc << std::endl;
			break;
		}

		len = m_stream->read(m_buf, INBUFF);
	}

	AudioSystem::RegisterStream(this);
	initialize(channels, rate);
	return true;
}

//the audio file did run out of samples, so fill it again
bool Mp3Stream::onGetData(Chunk& data)
{
	
	int ret = MPG123_NEED_MORE;
	
	if (m_handle && !m_isDone)
	{
		size_t size = 0;

		//i want data, what to do?
		while (ret == MPG123_NEED_MORE)
		{
            //wait for the decoder thread to finish it's work
			while (!m_hasData)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}

			ret = mpg123_read(m_handle, m_out, OUTBUFF, &size);
			off_t offset = mpg123_tell_stream(m_handle);
			if (offset == m_stream->getSize())
			{
				AudioSystem::UnregisterStream(this);
				return false;
			}
				
			if (ret == MPG123_OK)
				break;
			else if (ret == MPG123_ERR)
				return false;
		}

		data.samples = (short*)m_out;
		data.sampleCount = size / sizeof(short);
        //tell the decoder thread to make new data ready
		m_hasData = false;
		return true;
	}
	else
		return false;
}

//the audio wants to be seeked
void Mp3Stream::onSeek(sf::Time timeOffset)
{
	if (m_handle)
		mpg123_seek(m_handle, timeOffset.asSeconds(), 0);
}

//decode new samples from our decoder thread
void Mp3Stream::update()
{
	sf::Int64 len;
	if (!m_hasData)
	{
		len = m_stream->read(m_buf, INBUFF);
		if (mpg123_feed(m_handle, m_buf, len) == MPG123_DONE)
			m_isDone = true;
	
		m_hasData = true;
	}

}