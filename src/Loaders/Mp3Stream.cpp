#include "Mp3Stream.hpp"
#include <iostream>
#include <mpg123.h>

using namespace Loaders;


Mp3Stream::Mp3Stream() : m_handle(nullptr)
{

}

Mp3Stream::~Mp3Stream()
{
	stop();

	mpg123_close(m_handle);
	mpg123_delete(m_handle);

}

bool Mp3Stream::openFromStream(sf::InputStream& stream)
{
	stop();
	size_t size = 0;
	int ret = MPG123_NEED_MORE;
	int err = MPG123_OK;
	ssize_t len = 0;
	m_handle = mpg123_new(nullptr, &err);
	long rate;
	int channels, enc;
	m_stream = &stream;

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

	initialize(channels, rate);
	return true;
}

bool Mp3Stream::onGetData(Chunk& data)
{
	int ret = MPG123_NEED_MORE;
	sf::Lock lock(m_mutex);
	sf::Int64 len;

	if (m_handle)
	{
		size_t size = 0;

		//i want data, what to do?
		while (ret == MPG123_NEED_MORE)
		{
			ret = mpg123_read(m_handle, m_out, OUTBUFF, &size);
			if (ret == MPG123_OK)
				break;
			else if (ret == MPG123_ERR)
				return false;

			std::cout << "Loading mp3 stream" << std::endl;
			len = m_stream->read(m_buf, INBUFF);
			if (mpg123_feed(m_handle, m_buf, len) == MPG123_ERR)
				return false;
		}

		data.samples = (short*)m_out;
		data.sampleCount = size / sizeof(short);

		return (data.sampleCount > 0);
	}
	else
		return false;
}

void Mp3Stream::onSeek(sf::Time timeOffset)
{
	sf::Lock lock(m_mutex);

	if (m_handle)
		mpg123_seek(m_handle, timeOffset.asSeconds(), 0);
}
