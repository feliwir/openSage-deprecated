#pragma once
#include <fstream>
#include <stdint.h>

namespace Util
{
	template<class T>
	inline T Read(std::ifstream& fin)
	{
		T result;
		fin.read((char*)&result, sizeof(T));
		return result;
	}

	inline std::string ReadString(std::ifstream& fin)
	{
		std::string result;
		char c;
		while ((c=fin.get()))
			result.push_back(c);

		return result;
	}

	inline void Reverse(uint32_t& v)
	{
		v = (v << 24) | (v << 8 & 0xff0000) | (v >> 8 & 0xff00) | (v >> 24);
	}

	inline uint8_t* ReadBuffer(std::ifstream& fin,uint32_t size)
	{
		uint8_t* buf = new uint8_t[size];
		fin.read((char*)buf, size);
		return buf;
	}
}