// (C) Stephan Vedder 2015
#pragma once
#include <SFML/Audio.hpp>
#include <stdint.h>
#include "BigStream.hpp"

struct mpg123_handle_struct;
typedef mpg123_handle_struct mpg123_handle;

namespace Loaders
{
	class Mp3Stream : public sf::SoundStream
	{
	public:
		Mp3Stream();
		~Mp3Stream();

		bool open(const std::string& name);
		void update();
	protected:
		bool onGetData(Chunk& data);
		void onSeek(sf::Time timeOffset);

	private:
		mpg123_handle*      m_handle;
		bool				m_hasData;
		bool				m_isDone;
		BigStream			m_stream;
		#define INBUFF  16384
		#define OUTBUFF 32768 

		uint8_t m_buf[INBUFF];
		uint8_t m_out[OUTBUFF];

	};

} // namespace sfe