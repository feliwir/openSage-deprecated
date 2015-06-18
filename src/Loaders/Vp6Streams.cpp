#include "Vp6Stream.hpp"
#include "Util.hpp"
#include <iostream>

using namespace Util;
using namespace Loaders;
using namespace std;

#define MKTAG(a,b,c,d) (a|(b << 8)|(c << 16) |(d << 24))
#define MV0K_TAG MKTAG('M', 'V', '0', 'K')
#define MV0F_TAG MKTAG('M', 'V', '0', 'F')
#define MVhd_TAG MKTAG('M', 'V', 'h', 'd')

bool Vp6Stream::open(const std::string& name)
{
	uint32_t size = 0;
	uint32_t chunk_size = 0;
	m_fin.open(name,ios::binary);
	if (m_fin.fail())
		return false;

	if (Read<uint32_t>(m_fin) != MVhd_TAG)
	{
		cout << "Unsupported video format!" << endl;
		return false;
	}

	size = Read<uint32_t>(m_fin);
	m_header = Read<Vp6_EA_Header>(m_fin);
	m_fin.seekg(size, std::ios::beg);
	
	m_fps = m_header.denominator / m_header.numerator;
	
	if (Read<uint32_t>(m_fin) != MV0K_TAG)
	{
		cout << "Unsupported packet format!" << endl;
		return false;
	}

	chunk_size = Read<uint32_t>(m_fin);
	chunk_size -= 8;

	uint8_t* buf = ReadBuffer(m_fin, chunk_size);
	m_vp62.decodePacket(buf, chunk_size);
	delete[] buf;
	m_vp62.getImageSize((int*)&m_width, (int*)&m_height);
	cout << "Video width: "<<m_width << " height: " << m_height <<" fps: "<< m_fps << endl;

}

void Vp6Stream::play()
{
	uint32_t tag = 0;
	uint32_t chunk_size = 0;
	uint8_t* buf = nullptr;
	uint8_t* rgbaData = new uint8_t[4*m_width*m_height];
	uint32_t nFrame = 0;
	while(1)
	{
		tag = Read<uint32_t>(m_fin);

		if (tag != MV0K_TAG && tag!=MV0F_TAG)
		{
			cout << "Unsupported tag at position " << m_fin.tellg() << endl;
			return;
		}
		else
		{
			cout << "Valid tag at position "<<m_fin.tellg() << endl;
			chunk_size = Read<uint32_t>(m_fin);
			chunk_size -= 8;

			uint8_t* buf = ReadBuffer(m_fin, chunk_size);
			m_vp62.decodePacket(buf, chunk_size);
			m_vp62.getRGB(rgbaData);
			std::string name =  "Frame"+ std::to_string(nFrame)+".png";
			sf::Image img;
			img.create(m_width,m_height,rgbaData);
			img.saveToFile(name);
			delete[] buf;
		}

		++nFrame;
	}

	delete[] rgbaData;
}