// Stephan Vedder 2015
#include "BigStream.hpp"
#include "Util.hpp"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace Util;
using namespace Loaders;
std::vector<std::shared_ptr<BigStream::BigFile>> BigStream::archives;
std::map<std::string,std::shared_ptr<BigStream::BigEntry>> BigStream::entries;


BigStream::BigStream() : pos(0)
{
}

BigStream::~BigStream()
{
}

bool BigStream::open(const std::string& filename)
{
	entry = entries[filename];
	pos = 0;
	if (entry)
	{
		cout << "Opening BigEntry: " + filename << endl;
		return true;
	}
	else
		return false;
}

sf::Int64 BigStream::read(void* data,sf::Int64 size)
{
	sf::Int64 result = 0;
	entry->container->fin.seekg(entry->offset + pos, ios::beg);
	if (pos + size > entry->size)
		result = entry->size - pos;
	else
		result = size;
	entry->container->fin.read((char*)data, result);
	pos += result;
	return result;
}

sf::Int64 BigStream::seek(sf::Int64 newpos)
{
	if (newpos > entry->size)
		return -1;
	else
		pos = newpos;

	return pos;
}

sf::Int64 BigStream::tell()
{
	return pos;
}

sf::Int64 BigStream::getSize()
{
	return entry->size;
}

void BigStream::addFile(const string& name)
{
	auto big = make_shared<BigFile>();
	big->fin.open(name,ios::binary);

	if (big->fin.fail())
		return;

	big->header = Util::read<BigHeader>(big->fin);
	reverse(big->header.num_files);
	
	for (int i = 0; i < big->header.num_files; ++i)
	{
		auto entry = make_shared<BigEntry>();
		entry->offset = Util::read<uint32_t>(big->fin);
		reverse(entry->offset);
		entry->size = Util::read<uint32_t>(big->fin);
		reverse(entry->size);
		entry->name = Util::readString(big->fin);
		std::replace(entry->name.begin(),entry->name.end(),'\\', '/');
		entry->container = big;
		big->entries.push_back(entry);
		entries[entry->name] = entry;
	}

	archives.push_back(big);
}
