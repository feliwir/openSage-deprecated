// Stephan Vedder 2015
#pragma once
#include <stdint.h>
#include <string>

class Config
{
public:
	static const uint32_t width;
	static const uint32_t height;
	static const bool fullscreen;
	static const std::string title;
};