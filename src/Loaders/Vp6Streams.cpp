#include "Vp6Stream.hpp"
#include "Util.hpp"
#include <iostream>

using namespace Util;
using namespace Loaders;
using namespace std;

#define MKTAG(a,b,c,d) (a|(b << 8)|(c << 16) |(d << 24))
#define MV0K_TAG MKTAG('M', 'V', '0', 'K')
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
	m_vp62.decodePacket(buf, size);
	int width, height;
	m_vp62.getImageSize(&width, &height);
	int a = 0;
}

void Vp6Stream::play()
{

}