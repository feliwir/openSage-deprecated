// (C) Stephan Vedder 2015
#include "Vp6Stream.hpp"
#include "Util.hpp"
#include "../VideoSystem.hpp"
#include <iostream>
#include <libswscale/swscale.h>

using namespace Util;
using namespace Loaders;
using namespace std;

Vp6Stream::Vp6Stream() : m_fmtCtx(NULL), m_codecCtx(NULL), m_codec(NULL),
m_frame(NULL), m_curFrame(0), m_frameRGB(NULL), m_buf(NULL)
{
	m_running = false;
}

Vp6Stream::~Vp6Stream()
{
	if (m_thread.joinable())
		m_thread.join();

	av_freep(&m_frameRGB);
	av_freep(&m_frame);
	av_freep(&m_buf);	
	avcodec_close(m_codecCtx);
	avformat_close_input(&m_fmtCtx);
}

bool Vp6Stream::open(const std::string& name)
{

	AVCodecContext* codexCtxOrig = NULL;
	char errStr[256];
	unsigned int err = avformat_open_input(&m_fmtCtx, name.c_str(), NULL, NULL);
	if (err != 0)
	{
		av_strerror(err, errStr, 256);
		std::cout << "Error: " << errStr << std::endl;
		return false;
	}
		

	// Retrieve stream information
	if (avformat_find_stream_info(m_fmtCtx, NULL)<0)
		return false;

	// Find the first video stream
	for (uint8_t i = 0; i < m_fmtCtx->nb_streams; i++)
	{
		if (m_fmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_streamIndex = i;
			break;
		}
	}
	
	codexCtxOrig = m_fmtCtx->streams[m_streamIndex]->codec;

	// Find the decoder for the video stream
	m_codec = avcodec_find_decoder(codexCtxOrig->codec_id);
	if (m_codec == NULL)
	{
		return false; // Codec not found
	}
	// Copy context
	m_codecCtx = avcodec_alloc_context3(m_codec);
	if (avcodec_copy_context(m_codecCtx, codexCtxOrig) != 0)
	{
		return false; // Error copying codec context
	}

	// Open codec
	if (avcodec_open2(m_codecCtx, m_codec,NULL)<0)
		return false; // Could not open codec

	m_frame = av_frame_alloc();

	m_frameRGB = av_frame_alloc();

	// Determine required buffer size and allocate buffer
	int numBytes = avpicture_get_size(PIX_FMT_RGBA, m_codecCtx->width, m_codecCtx->height);
	m_buf = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
	avpicture_fill((AVPicture *)m_frameRGB, m_buf, PIX_FMT_RGBA,m_codecCtx->width, m_codecCtx->height);

	// initialize SWS context for software scaling
	m_swsCtx = sws_getContext(m_codecCtx->width,
		m_codecCtx->height,
		m_codecCtx->pix_fmt,
		m_codecCtx->width,
		m_codecCtx->height,
		PIX_FMT_RGBA,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
		);

	//Fill the texture with a black image
	sf::Image img;
	img.create(m_codecCtx->width, m_codecCtx->height);
	m_tex.loadFromImage(img);
	m_tex.setSmooth(true);
	m_status = Stopped;
	return true;
}

void Vp6Stream::play()
{
	if (m_status == Playing)
		return;

	m_status = Playing;
	m_running = true;
	m_thread = std::thread(&Vp6Stream::update, this);
	
}

void Vp6Stream::update()
{
	AVPacket packet;
	int frameFinished;
	m_curFrame = false;
	double fps = m_codecCtx->time_base.den / m_codecCtx->time_base.num;
	auto frameLength = std::chrono::milliseconds((int)(1/fps* 1000));
	auto last = std::chrono::high_resolution_clock::now();

	while (av_read_frame(m_fmtCtx, &packet) >= 0)
	{
		if (m_running == false)
		{
			av_free_packet(&packet);
			break;
		}
			

		// Is this a packet from the video stream?
		if (packet.stream_index == m_streamIndex)
		{			
			// Decode video frame
			avcodec_decode_video2(m_codecCtx, m_frame, &frameFinished, &packet);

			// Did we get a video frame?
			if (frameFinished)
			{
				// Convert the image from its native format to RGB
				sws_scale(m_swsCtx, (uint8_t const * const *)m_frame->data,
					m_frame->linesize, 0, m_codecCtx->height,
					m_frameRGB->data, m_frameRGB->linesize);

				m_tex.update((sf::Uint8*)m_frameRGB->data[0]);
				last += frameLength;
				std::this_thread::sleep_until(last);
				++m_curFrame;
			}
		}
		// Free the packet that was allocated by av_read_frame
		av_free_packet(&packet);	
	}

	std::cout << "End stream!" << std::endl;
	m_status = Stopped;	
}

