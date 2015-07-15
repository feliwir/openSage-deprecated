// Stephan Vedder 2015
#pragma once
#include <fstream>
#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <SFML/System.hpp>

namespace Util
{
	template<class T>
	inline T Read(std::ifstream& fin)
	{
		T result;
		fin.read((char*)&result, sizeof(T));
		return result;
	}

	template<class T>
	inline T Read(uint8_t*& buf)
	{
		T result;
		result = *(T*)buf;
		buf += sizeof(T);
		return result;
	}

	inline std::string ReadString(std::ifstream& fin)
	{
		std::string result;
		char c;
		while ((c = fin.get()))
			result.push_back(c);

		return result;
	}

	inline void Reverse(uint32_t& v)
	{
		v = (v << 24) | (v << 8 & 0xff0000) | (v >> 8 & 0xff00) | (v >> 24);
	}

	inline uint8_t* ReadBuffer(std::ifstream& fin, uint32_t size)
	{
		uint8_t* buf = new uint8_t[size];
		fin.read((char*)buf, size);
		return buf;
	}

    inline std::string ReadAll(std::shared_ptr<sf::InputStream> stream)
    {
        char* buf = new char[stream->getSize()+1];
        stream->read(buf, stream->getSize());
        buf[stream->getSize()] = 0;
        return buf;
    }
}
