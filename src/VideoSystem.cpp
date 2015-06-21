#include "VideoSystem.hpp"
#include <libavcodec/avcodec.h>
#include <iostream>


void VideoSystem::Initialize()
{
	av_register_all();
}

void VideoSystem::Release()
{
}
