// Stephan Vedder 2015
#pragma once
#include <string>
#include <stdint.h>
#include <atomic>
#include <thread>
#include <glm/glm.hpp>
#include "../Graphics/Texture.hpp"

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

		enum Status
		{
			Stopped = 0,
			Playing = 1
		};

		bool open(const std::string& name);
		void play();
		void update();

		inline void stop()
		{
			m_status = Stopped;
			m_running = false;
			if (m_thread.joinable())
				m_thread.join();
		}

		inline const uint8_t* getColorData()
		{
            return m_frameRGB->data[0];
		}

		inline Status getStatus()
		{
			return m_status;
		}

        inline glm::vec2 getSize()
        {
            return glm::vec2(m_codecCtx->width, m_codecCtx->height);
        }

        inline bool isUpdated()
        {
            if (m_updated)
            {
                m_updated = false;
                return true;
            }
            return false;
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
		std::atomic_bool m_running;
        std::atomic_bool m_updated;
		Status m_status;
	};
}