// Stephan Vedder 2015
#include "VideoSystem.hpp"
#include <libavcodec/avcodec.h>
#include <iostream>


void VideoSystem::initialize()
{
	av_log_set_level(AV_LOG_QUIET);
	av_register_all();
}

void VideoSystem::release()
{
}
