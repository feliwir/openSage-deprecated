#include "VideoSystem.hpp"
#include <libavcodec/avcodec.h>
#include <iostream>


void VideoSystem::Initialize()
{
	av_log_set_level(AV_LOG_QUIET);
	av_register_all();
}

void VideoSystem::Release()
{
}
