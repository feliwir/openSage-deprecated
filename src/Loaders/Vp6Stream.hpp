#pragma once
#include "VP62.hpp"
#include <string>
#include <fstream>
#include <stdint.h>
namespace Loaders
{
class Vp6Stream
{
private:
	struct Vp6_EA_Header
	{
		uint32_t unused;
		uint32_t unused2;
		uint32_t frameCount;
		uint32_t largestChunk;
		uint32_t denominator;
		uint32_t numerator;
	};
public:
	bool open(const std::string& name);
	void play();
	inline void getRGBA(unsigned char* rgba)
	{
		m_vp62.getRGB(rgba);
	}
private:
	std::ifstream m_fin;
	Vp6_EA_Header m_header;
	VP62 m_vp62;
	double m_fps;
};
}