#pragma once
#include <string>
#include <stdint.h>
#include <mutex>
#include <thread>
#include <SFML/Graphics.hpp>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


namespace Loaders
{
class Vp6Stream
{
public:
	Vp6Stream();
	~Vp6Stream();

	bool open(const std::string& name);
	void play();
	void update();

	inline void stop()
	{
		m_status = Stopped;
	}

	inline sf::Texture& GetTexture()
	{
		return m_tex;
	}

	enum Status
	{
		Stopped = 0,
		Playing = 1
	};

	inline Status getStatus()
	{
		return m_status;
	}
private:
	AVFormatContext* m_fmtCtx;
	AVCodecContext* m_codecCtx;
	AVCodec* m_codec;
	AVFrame* m_frame;
	AVFrame* m_frameRGB;
	SwsContext* m_swsCtx;
	uint8_t* m_buf;
	int8_t m_streamIndex;
	uint32_t m_curFrame;
	std::thread m_thread;
	sf::Texture m_tex;
	bool m_running;
	Status m_status;
};
}