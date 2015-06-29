#include <iostream>
#include <string>
#include "AptFile.hpp"
#include "Util.hpp"
#include "BigStream.hpp"
#include "../Game/ActionScript.hpp"

using namespace Loaders;
using namespace Util;

bool AptFile::loadFromStream(sf::InputStream& aptStream, sf::InputStream& constStream,const std::string& name)
{
	
	uint8_t* constBuf = new uint8_t[constStream.getSize()];
	constStream.read(constBuf, constStream.getSize());

	//now start parsing by setting our iter to the start of the aptbuffer
	uint8_t* iter = constBuf;
	//skip the first 20 bytes
	iter += 0x14;
	m_data.aptdataoffset = Read<uint32_t>(iter);
	m_data.itemcount = Read<uint32_t>(iter);
	//skip 4 bytes now
	iter += 4;

	//now read each item
	for (uint32_t i = 0; i < m_data.itemcount; ++i)
	{
		m_data.items.push_back(ConstItem());
		m_data.items[i].type = static_cast<ConstItemType>(Read<uint32_t>(iter));
		auto value  = Read<uint32_t>(iter);
		if (m_data.items[i].type == TYPE_STRING)
		{
			//read the string from the position specified
			m_data.items[i].strvalue = (char *)(constBuf + (uintptr_t)value);
		}
		else
		{
			m_data.items[i].numvalue = value;
		}
	}

	std::cout << "Parsed " << name << ".const";
	delete[] constBuf;

	uint8_t* aptBuf = new uint8_t[aptStream.getSize()];
	aptStream.read(aptBuf, aptStream.getSize());
	
	iter = aptBuf + m_data.aptdataoffset;
	ParseCharacter(iter,aptBuf);

	std::cout << "Parsed " << name << ".apt" << std::endl;

	BigStream datStream;
	if(!datStream.open(name+".dat"))
	{
		//no dat file present
		return true;
	}
	std::cout << "Parsed " << name << ".dat" << std::endl;
	uint8_t* datBuf = new uint8_t[datStream.getSize()+1];
	datStream.read(datBuf, datStream.getSize());
	datBuf[datStream.getSize()] = 0;

	std::string content = (char*)datBuf;
	std::istringstream iss(content);
	std::string line,strInt;
	int n1 = 0, n2 = 0, pos;
	while (std::getline(iss, line))
	{
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
		if (line[0] == ';')
			continue;

		pos = line.find("->");
		strInt = line.substr(0, pos);
		n1 = std::atoi(strInt.c_str());
		strInt = line.substr(pos+2, line.size());
		n2 = std::atoi(strInt.c_str());
		m_dat[n1] = n2;
	}

	delete[] datBuf;

	for (auto& i : m_dat)
	{
		//insert texture if not in already
		if (m_textures.find(i.second) == m_textures.end())
		{
			BigStream texStream;
			if (!texStream.open("art/Textures/apt_" + name + "_"+std::to_string(i.second) + ".tga"))
				continue;

			sf::Image img;
			img.loadFromStream(texStream);

			std::shared_ptr<sf::Texture> tex = std::make_shared <sf::Texture>();
			tex->loadFromImage(img);
			m_textures[i.second] = tex;
		}
	}
	return true;
}

AptFile::GeometryEntry AptFile::ParseGeometry(const std::string& name)
{
	GeometryEntry entry;
	BigStream ruStream;

	if (!ruStream.open(name))
	{
		std::cout << "Failed to load ru file: " << name << std::endl;
		//no ru file present
		return entry;
	}

	uint8_t* ruBuf = new uint8_t[ruStream.getSize() + 1];
	ruStream.read(ruBuf, ruStream.getSize());
	ruBuf[ruStream.getSize()] = 0;

	std::string content = (char*)ruBuf;
	std::vector<std::string> params;
	std::istringstream iss(content);
	std::string line;
	std::string param;
	int n1 = 0, n2 = 0, pos;
	while (std::getline(iss, line))
	{
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(0, line.find_first_not_of(" \t"));
		if (line[0] == ';')
			continue;

		param = line.substr(1, line.size());
		param.erase(std::remove(param.begin(), param.end(), ' '), param.end());
		
		std::stringstream splitter(param);
		while (std::getline(splitter, param,':'))
		{
			params.push_back(param);
		}

		switch (line[0])
		{
			case 'c':
			{
				
			}
				break;
			case 's':
			{
				if (params[0] == "tc")
				{
					TextureStyle ts;
					ts.red = std::atoi(params[1].c_str());
					ts.green = std::atoi(params[2].c_str());
					ts.blue = std::atoi(params[3].c_str());
					ts.alpha = std::atoi(params[4].c_str());
					ts.TextureCharacter = std::atoi(params[5].c_str());
					ts.rotateandscale ={std::atof(params[6].c_str()), std::atof(params[7].c_str()),
										std::atof(params[8].c_str()), std::atof(params[9].c_str())};
					ts.translate = {std::atof(params[10].c_str()), std::atof(params[11].c_str())};
					entry.texturestyles.push_back(ts);
				}
				else if (params[0] == "s")
				{
					SolidStyle s;
					s.red = std::atoi(params[1].c_str());
					s.green = std::atoi(params[2].c_str());
					s.blue = std::atoi(params[3].c_str());
					s.alpha = std::atoi(params[4].c_str());
					entry.solidstyles.push_back(s);
				}
			}
				break;
			case 't':
			{
				Tri t;
				t.v1 = { std::atof(params[0].c_str()), std::atof(params[1].c_str())};
				t.v2 = { std::atof(params[2].c_str()), std::atof(params[3].c_str())};
				t.v3 = { std::atof(params[4].c_str()), std::atof(params[5].c_str())};
				entry.tris.push_back(t);
			}
				break;
			case 'l':
			{
				Line l;
				l.v1 = { std::atof(params[0].c_str()), std::atof(params[1].c_str())};
				l.v2 = { std::atof(params[2].c_str()), std::atof(params[3].c_str())};
				entry.lines.push_back(l);
			}
		}

		params.clear();
	}

	entry.tricount = entry.tris.size();
	entry.linecount = entry.lines.size();
	entry.texturestylecount = entry.texturestyles.size();
	entry.solidstylecount = entry.solidstyles.size();
	entry.linestylecount = entry.linestyles.size();

	return entry;
}

std::shared_ptr<AptFile::Character> AptFile::ParseCharacter(uint8_t*& buf,uint8_t* base)
{
	auto type = Read<uint32_t>(buf);
	auto signature = Read<uint32_t>(buf);
	std::shared_ptr<Character> result;
	
	switch (type)
	{
		case MOVIE:
		{
		auto movie = std::make_shared<Movie>();
		movie->type = type;
		movie->signature = signature;
		movie->framecount = Read<uint32_t>(buf);

		auto frameOffset = Read<uint32_t>(buf)+base;
		for (auto i = 0; i < movie->framecount; ++i)
		{
			Frame f;
			f.frameitemcount = Read<uint32_t>(frameOffset);
			auto itemsOffset = Read<uint32_t>(frameOffset)+base;
			for (auto j = 0; j < f.frameitemcount;  ++j)
			{
				auto itemOffset = Read<uint32_t>(itemsOffset)+base;
				f.frameitems.push_back(ParseFrameItem(itemOffset, base));
			}
		}

		result = movie;
		}
		break;
	}

	return result;
}

std::shared_ptr<AptFile::FrameItem> AptFile::ParseFrameItem(uint8_t*& buf, uint8_t* base)
{
	auto type = Read<uint32_t>(buf);
	std::shared_ptr<FrameItem> result;

	switch (type)
	{
	case ACTION:
	{
		auto action = std::make_shared<Action>();
		action->type = type;
		auto actionOffset = base + Read<uint32_t>(buf);
		auto actionSize = Script::AS::GetBytecodeSize(actionOffset);
		action->bytecode = new uint8_t[actionSize];
		std::copy(actionOffset, actionOffset + actionSize, action->bytecode);
		result = action;
	}
		break;
	case PLACEOBJECT:
	{
		auto po = std::make_shared<PlaceObject>();
		po->type = type;
		po->flags =	Read<uint32_t>(buf);
		po->depth =	Read<int32_t>(buf);
		po->character = Read<int32_t>(buf);
		po->rotateandscale = Read<Transform>(buf);
		po->translate = Read<Vector2>(buf);
		po->red = Read<uint8_t>(buf);
		po->green = Read<uint8_t>(buf);
		po->blue = Read<uint8_t>(buf);
		po->alpha = Read<uint8_t>(buf);
		buf += 4;
		po->ratio = Read<float>(buf);
		char* name = reinterpret_cast<char*>(Read<uint32_t>(buf)+base);
		po->name = std::string(name);
		po->clipdepth = Read<uint32_t>(buf);
		auto poaOffset = Read<uint32_t>(buf) +base;
		po->poa.clipactioncount = Read<uint32_t>(poaOffset);
		auto poaOffset2 = Read<uint32_t>(poaOffset) +base;
	}
		break;
	case BACKGROUNDCOLOR:
	{
		auto bgcolor = std::make_shared<BackgroundColor>();
		bgcolor->type = type;
		bgcolor->red = Read<uint8_t>(buf);
		bgcolor->green = Read<uint8_t>(buf);
		bgcolor->blue = Read<uint8_t>(buf);
		bgcolor->alpha = Read<uint8_t>(buf);
		result = bgcolor;
	}
		break;
	}

	return result;
}

void AptFile::Update()
{
	
}

void AptFile::Render(sf::RenderWindow& win)
{

}

AptFile::~AptFile()
{

}

AptFile::AptFile() :  m_frame(0)
{
}