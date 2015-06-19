#include "Video.hpp"
#include <libavcodec/avcodec.h>
#include <iostream>


void Video::Initialize()
{
	av_register_all();
}

void Video::Release()
{
}
