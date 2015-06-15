#pragma once
#include <SFML/Audio.hpp>
#include <mpg123.h>

namespace Loaders
{
	class Mp3Stream : public sf::SoundStream
	{
	public:
		Mp3Stream();
		~Mp3Stream();

		bool openFromStream(sf::InputStream& stream);
	protected:
		bool onGetData(Chunk& data);
		void onSeek(sf::Time timeOffset);

	private:
		mpg123_handle*      m_handle;
		sf::Mutex           m_mutex;
		sf::InputStream*	m_stream;
		#define INBUFF  16384
		#define OUTBUFF 32768 

		uint8_t m_buf[INBUFF];
		uint8_t m_out[OUTBUFF];

	};

} // namespace sfe