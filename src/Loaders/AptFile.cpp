// Stephan Vedder 2015
#include <iostream>
#include <string>
#include "AptFile.hpp"
#include "Util.hpp"
#include "BigStream.hpp"
#include "../Script/ActionScript.hpp"
#include "../Game/GameData.hpp"

using namespace Loaders;
using namespace Util;

#define CHAR_SIG 0x09876543

bool AptFile::loadFromStream(sf::InputStream& aptStream, sf::InputStream& constStream,const std::string& name)
{
	m_name = name;
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
			m_data.items[i].strvalue = std::string(reinterpret_cast<char*>(constBuf+value));
		}
		else
		{
			m_data.items[i].numvalue = value;
		}
	}

	std::cout << "Parsed " << name << ".const";
	delete[] constBuf;

	m_aptBuf = new uint8_t[aptStream.getSize()];
	aptStream.read(m_aptBuf, aptStream.getSize());
	
	iter = m_aptBuf + m_data.aptdataoffset;
	m_characters[0] = ParseCharacter(iter, m_aptBuf);
	DisplayItem di;
	di.ch = 0;
	m_displaylist[0] = di;

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
		if (pos != std::string::npos)
		{
		strInt = line.substr(0, pos);
		n1 = std::stoi(strInt);
		strInt = line.substr(pos+2, line.size());
		n2 = std::stoi(strInt);
		m_dat[n1] = n2;
		}
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
			tex->setSmooth(true);
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
	bool valid = false;
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
					entry.order.push_back(CLEAR);
			}
				break;
			case 's':
			{
				if (params[0] == "tc")
				{
					TextureStyle ts;
					ts.color = {std::stoi(params[1]), std::stoi(params[2]), 
								std::stoi(params[3]), std::stoi(params[4])};
					ts.tex = std::stoi(params[5]);
					ts.rotateandscale ={std::stof(params[6]), std::stof(params[7]),
										std::stof(params[8]), std::stof(params[9])};
					ts.translate = {std::stof(params[10]), std::stof(params[11])};
					entry.texturestyles.push_back(ts);
					entry.order.push_back(TEXTURSTYLE);
				}
				else if (params[0] == "s")
				{
					SolidStyle s;
					s.color = { std::stoi(params[1]), std::stoi(params[2]),
								std::stoi(params[3]), std::stoi(params[4])};
					entry.solidstyles.push_back(s);
					entry.order.push_back(SOLIDSTYLE);
				}
				else if (params[0] == "l")
				{
					LineStyle l;
					l.width = std::stoi(params[1]);
					l.color = { std::stoi(params[2]), std::stoi(params[3]),
						std::stoi(params[4]), std::stoi(params[5]) };
					entry.linestyles.push_back(l);
					entry.order.push_back(LINESTYLE);
				}
			}
				break;
			case 't':
			{
				Tri t;
				t.v1 = { std::stof(params[0]), std::stof(params[1])};
				t.v2 = { std::stof(params[2]), std::stof(params[3])};
				t.v3 = { std::stof(params[4]), std::stof(params[5])};
				entry.tris.push_back(t);
				entry.order.push_back(TRIANGLE);
			}
				break;
			case 'l':
			{
				Line l;
				l.v1 = { std::stof(params[0]), std::stof(params[1])};
				l.v2 = { std::stof(params[2]), std::stof(params[3])};
				entry.lines.push_back(l);
				entry.order.push_back(LINE);
			}
		}

		params.clear();
	}

		
	return entry;
}

std::shared_ptr<AptFile::Character> AptFile::ParseCharacter(uint8_t*& buf,uint8_t* base)
{
	uint8_t* start = buf;
	auto type = Read<uint32_t>(buf);
	auto signature = Read<uint32_t>(buf);
	if (signature != CHAR_SIG)
		return nullptr;

	std::shared_ptr<Character> result;
	
	switch (type)
	{
		case SHAPE:
		{
			auto sh = std::make_shared<Shape>();
			sh->bounds = Read<glm::f32vec4>(buf);
			sh->geometry = Read<uint32_t>(buf);
			m_geometry[sh->geometry] = ParseGeometry(m_name + "_geometry/" + std::to_string(sh->geometry) + ".ru");
			result = sh;
		}
			break;
		case EDITTEXT:
		{
			auto et = std::make_shared<EditText>();
			et->bounds = Read<glm::f32vec4>(buf);
			et->font = Read<uint32_t>(buf);
			et->alignment = Read<uint32_t>(buf);
			et->color = Read<glm::u8vec4>(buf);
			et->fontheight = Read<float>(buf);
			et->readonly = Read<uint32_t>(buf);
			et->multiline = Read<uint32_t>(buf);
			et->wordwrap = Read<uint32_t>(buf);
			char* text = reinterpret_cast<char*>(Read<uint32_t>(buf)+base);
			et->text = std::string(text);
			char* variable = reinterpret_cast<char*>(Read<uint32_t>(buf)+base);
			et->variable = std::string(variable);

			result = et;
		}
			break;
		case FONT:
		{
			auto font = std::make_shared<Font>();
			char* name = reinterpret_cast<char*>(Read<uint32_t>(buf)+base);
			font->name = std::string(name);
			font->glyphcount = Read<uint32_t>(buf);
			auto glyphOffset = Read<uint32_t>(buf)+base;
			for (auto i = 0; i < font->glyphcount; ++i)
			{
				font->glyphs.push_back(Read<uint32_t>(buf));
			}

		
			result = font;
		}
			break;
		case SPRITE:
		{
			auto sprite = std::make_shared<Sprite>();
			sprite->framecount = Read<uint32_t>(buf);
			auto frameOffset = Read<uint32_t>(buf)+base;
			for (auto i = 0; i < sprite->framecount; ++i)
			{
				Frame f;
				f.frameitemcount = Read<uint32_t>(frameOffset);
				auto itemsOffset = Read<uint32_t>(frameOffset)+base;
				for (auto j = 0; j < f.frameitemcount; ++j)
				{
					auto itemOffset = Read<uint32_t>(itemsOffset)+base;
					f.frameitems.push_back(ParseFrameItem(itemOffset, base));
				}

				sprite->frames.push_back(f);
			}

			sprite->cFrame = Read<uint32_t>(buf);
			result = sprite;
		}
			break;
		case MOVIE:
		{
			auto movie = std::make_shared<Movie>();
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
				movie->frames.push_back(f);
			}

			movie->cFrame = Read<uint32_t>(buf);
			movie->charactercount = Read<uint32_t>(buf);
			auto charactersOffset = Read<uint32_t>(buf) + base;
			for (auto i = 0; i < movie->charactercount; ++i)
			{
				auto fileOffset = Read<uint32_t>(charactersOffset);
				if (fileOffset)
				{
					auto characterOffset = fileOffset+base;
					//movies contain themselves so skip them
					if (characterOffset == start)
						continue;

					m_characters[i] = ParseCharacter(characterOffset, base);
				}
				else
				{
					m_characters[i] = nullptr;
				}
			}
			movie->width = Read<uint32_t>(buf);
			movie->height = Read<uint32_t>(buf);
			result = movie;
		}
		break;
	}

	if (result != nullptr)
	{
		result->type = type;
		result->signature = signature;
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
		auto actionOffset = base + Read<uint32_t>(buf);
		auto actionSize = Script::AS::GetBytecodeSize(actionOffset);
		action->bytecode = new uint8_t[actionSize];
		std::copy(actionOffset, actionOffset + actionSize, action->bytecode);
		result = action;
	}
		break;
	case FRAMELABEL:
	{
		auto fl = std::make_shared<FrameLabel>();
		char* lbl = reinterpret_cast<char*>(Read<uint32_t>(buf)+base);
		fl->label = std::string(lbl);
		fl->flags = Read<uint32_t>(buf);
		fl->frame = Read<uint32_t>(buf);
		result = fl;
	}
		break;
	case PLACEOBJECT:
	{
		auto po = std::make_shared<PlaceObject>();
		uint32_t flags = Read<uint32_t>(buf);
		po->flags = *(PoFlags*)(&flags);
		po->depth =	Read<int32_t>(buf);
		po->character = Read<int32_t>(buf);
		po->rotateandscale = Read<glm::f32mat2>(buf);
		po->translate = Read<glm::f32vec2>(buf);
		po->color = Read<glm::u8vec4>(buf);
		buf += 4;
		po->ratio = Read<float>(buf);
		char* name = reinterpret_cast<char*>(Read<uint32_t>(buf)+base);
		po->name = std::string(name);
		po->clipdepth = Read<uint32_t>(buf);
		auto poaOffset = Read<uint32_t>(buf) +base;
		po->poa.clipactioncount = Read<uint32_t>(poaOffset);
		auto poaOffset2 = Read<uint32_t>(poaOffset) +base;
		result = po;
	}
		break;
	case REMOVEOBJECT:
	{
		auto ro = std::make_shared<RemoveObject>();
		ro->depth = Read<int32_t>(buf);
		result = ro;
	}
		break;
	case BACKGROUNDCOLOR:
	{
		auto bgcolor = std::make_shared<BackgroundColor>();
		bgcolor->color = Read<glm::u8vec4>(buf);
		result = bgcolor;
	}
		break;
	}

	if (result != nullptr)
		result->type = type;

	return result;
}

void AptFile::Update(std::map<uint32_t, DisplayItem>& displaylist, std::vector<uint32_t>& deleteList)
{
	std::cout << "Updating frame " << m_frame << std::endl;

	deleteList.clear();
	for (auto& pair : displaylist)
	{
		auto& di = pair.second;
		auto& ch = m_characters[di.ch];
		if (ch == nullptr)
			continue;

		switch (ch->type)
		{
		case MOVIE:
		{
			auto m = std::static_pointer_cast<Movie>(ch);
			m->cFrame %= m->framecount;
			auto& f = m->frames[m->cFrame];
			UpdateFrame(f,di);
			Update(m->displayList,m->deleteList);
			++m->cFrame;

		}
			break;
		case SPRITE:
		{
			auto sp = std::static_pointer_cast<Sprite>(ch);
			sp->cFrame %= sp->framecount;
			auto& f = sp->frames[sp->cFrame];
			UpdateFrame(f,di);
			Update(sp->displayList, sp->deleteList);
			++sp->cFrame;
		}
			break;
		}
	}

	for (auto& ro : deleteList)
	{
		displaylist.erase(ro);
	}

	++m_frame;
}

void AptFile::UpdateFrame(Frame& frame, DisplayItem& parent)
{
	for (auto& fi : frame.frameitems)
	{
		if (fi == nullptr)
			continue;

		auto sp = std::static_pointer_cast<Sprite>(m_characters[parent.ch]);

		switch (fi->type)
		{
		case PLACEOBJECT:
		{
			auto po = std::static_pointer_cast<PlaceObject>(fi);

			if (po->character < 0)
			{
				std::cout << "Editing displayItem at depth " << po->depth << std::endl;
				
				auto di = sp->displayList[po->depth];
				std::cout << "Transforming character " << di.ch << " (" 
							<< po->translate.x<< ","<< po->translate.y << ")" << std::endl;
				
				if (po->flags.PlaceFlagMove)
					di.translate = po->translate;
				
				if (po->flags.PlaceFlagHasMatrix)
					di.rotscale = po->rotateandscale;

				sp->displayList[po->depth] = di;
			}
			else
			{
				std::cout << "Creating displayItem at depth " << po->depth << " from character "<< po->character << std::endl;
				DisplayItem di;
				di.ch = po->character;

				di.color = po->color;
				std::cout << "Transforming character " << di.ch << " (" 
							<< po->translate.x<< ","<< po->translate.y << ")" << std::endl;

				di.translate = po->translate;
				di.rotscale = po->rotateandscale;
				sp->displayList[po->depth] = di;
			}
		}
			break;
		case REMOVEOBJECT:
		{
			auto ro = std::static_pointer_cast<RemoveObject>(fi);
			std::cout << "Delete diplayItem at depth " << ro->depth << std::endl;
			m_deleteList.push_back(ro->depth);
		}
			break;
		case ACTION:
		{
			auto action = std::static_pointer_cast<Action>(fi);
			std::cout << "Execute action"  << std::endl;
			Script::AS::ExecuteBytecode(action->bytecode,parent,m_data,m_aptBuf);
			
		}
			break;
		}
	}
}

void AptFile::Render(sf::RenderWindow& win, std::map<uint32_t, DisplayItem>& displaylist, glm::vec2 offset, glm::f32mat2 rotscale)
{
	sf::View v;
	v.setCenter(sf::Vector2f(512, 384));
	v.setSize(sf::Vector2f(1024, 768));
	win.setView(v);

	for (auto& pair : displaylist)
	{
		auto& di = pair.second;
		auto& ch = m_characters[di.ch];
		if (ch == nullptr)
			continue;

		switch (ch->type)
		{
		case SPRITE:
		{
			auto sp = std::static_pointer_cast<Sprite>(ch);
			Render(win, sp->displayList, offset + di.translate, rotscale*di.rotscale);
		}
			break;
		case MOVIE:
		{
			auto m = std::static_pointer_cast<Movie>(ch);
			Render(win, m->displayList, offset + di.translate, rotscale*di.rotscale);
		}
			break;
		case SHAPE:
		{
			auto sh = std::static_pointer_cast<Shape>(ch);
			auto& geometry = m_geometry[sh->geometry];
			sf::VertexArray va;
			RenderGeometry(win, di,offset,rotscale);
		}
			break;
		case EDITTEXT:
		{
			auto et = std::static_pointer_cast<EditText>(ch);
			std::cout << "Rendering edittext: " << di.ch << std::endl;
			//TODO render here
			auto font = std::static_pointer_cast<Font>(m_characters[et->font]);
			auto stream = *GameData::GetFont(font->name);
			sf::Font data;
			data.loadFromStream(stream);
			sf::Text text(et->text, data,et->fontheight);
			auto pos = glm::vec2(offset.x + et->bounds.x, +offset.y + et->bounds.y);
			text.setPosition(pos.x, pos.y);
			text.setColor(sf::Color(et->color.r, et->color.g, et->color.b, et->color.a));
			win.draw(text);
		}
			break;
		}
	}
}

void AptFile::RenderGeometry(sf::RenderWindow& win, DisplayItem& di, glm::f32vec2 offset, glm::f32mat2 rotscale)
{
	uint32_t cTri = 0, cLine = 0, cSS = 0, cTS = 0, cLS = 0;
	SolidStyle ss;
	TextureStyle ts;
	LineStyle ls;
	Style cS;
	auto& ch = m_characters[di.ch];
	
	auto sh = std::static_pointer_cast<Shape>(ch);
	auto& geometry = m_geometry[sh->geometry];

	for (auto& c : geometry.order)
	{
		switch (c)
		{
		case CLEAR:
		{
			ls.width = 0;
			ls.color = { 0, 0, 0, 0 };
			ss.color = { 0, 0, 0, 0 };
			ts.color = { 0, 0, 0, 0 };
			ts.rotateandscale = { 0.0, 0.0, 0.0, 0.0 };
			ts.tex = 0;
			ts.translate = { 0.0, 0.0 };
			di.texture = nullptr;
		}
			break;
		case SOLIDSTYLE:
		{
			cS = STYLE_SOLID;
			ss = geometry.solidstyles[cSS];
			di.texture = nullptr;
			++cSS;
		}
			break;
		case LINESTYLE:
		{
			cS = STYLE_LINE;
			ls = geometry.linestyles[cLS];
			di.texture = nullptr;
			++cLS;
		}
			break;
		case TEXTURSTYLE:
		{
			cS = STYLE_TEXTURE;
			ts = geometry.texturestyles[cTS];
			auto id = m_dat[ts.tex];
			auto tex = m_textures[id];
			di.texture = tex;
			++cTS;
		}
			break;
		case TRIANGLE:
		{
			auto t = geometry.tris[cTri];
			sf::Color c;
			glm::f32vec2 uv1, uv2, uv3;

			if (cS == STYLE_SOLID)
			{
				c.r = ss.color.r;
				c.g = ss.color.g;
				c.b = ss.color.b;
				c.a = ss.color.a;
			}
			else if (cS == STYLE_LINE)
			{
				c.r = ls.color.r;
				c.g = ls.color.g;
				c.b = ls.color.b;
				c.a = ls.color.a;
			}
			else if (cS == STYLE_TEXTURE)
			{
				c.r = ts.color.r;
				c.g = ts.color.g;
				c.b = ts.color.b;
				c.a = ts.color.a;

				uv1 = t.v1 + ts.translate;
				uv2 = t.v2 + ts.translate;
				uv3 = t.v3 + ts.translate;
			}

			float minX = std::fmin(t.v1.x, std::fmin(t.v2.x, t.v3.x));
			float minY = std::fmin(t.v1.y, std::fmin(t.v2.y, t.v3.y));
			auto minVec = glm::f32vec2(minX, minY);

			auto vec1 = t.v1 - minVec;
			auto vec2 = t.v2 - minVec;
			auto vec3 = t.v3 - minVec;

			vec1 = di.rotscale * rotscale * vec1;
			vec2 = di.rotscale * rotscale * vec2;
			vec3 = di.rotscale * rotscale * vec3;

			vec1 += di.translate + offset;
			vec2 += di.translate + offset;
			vec3 += di.translate + offset;

			sf::VertexArray va;
			va.setPrimitiveType(sf::Triangles);

			va.append(sf::Vertex(sf::Vector2f(vec1.x, vec1.y), c, sf::Vector2f(uv1.x, uv1.y)));
			va.append(sf::Vertex(sf::Vector2f(vec2.x, vec2.y), c, sf::Vector2f(uv2.x, uv2.y)));
			va.append(sf::Vertex(sf::Vector2f(vec3.x, vec3.y), c, sf::Vector2f(uv3.x, uv3.y)));

			win.draw(va,di.texture.get());
			++cTri;
		}
			break;
		case LINE:
		{
			auto l = geometry.lines[cLine];
			sf::Color c;

			if (cS == STYLE_SOLID)
			{
				c.r = ss.color.r;
				c.g = ss.color.g;
				c.b = ss.color.b;
				c.a = ss.color.a;
			}
			else if (cS == STYLE_LINE)
			{
				c.r = ls.color.r;
				c.g = ls.color.g;
				c.b = ls.color.b;
				c.a = ls.color.a;
			}
			else if (cS == STYLE_TEXTURE)
			{
				c.r = ts.color.r;
				c.g = ts.color.g;
				c.b = ts.color.b;
				c.a = ts.color.a;
			}

			auto vec1 = sf::Vector2f(l.v1.x, l.v1.y);
			auto vec2 = sf::Vector2f(l.v2.x, l.v2.y);

			sf::VertexArray va;
			va.setPrimitiveType(sf::Lines);
		
			va.append(sf::Vertex(vec1, c));
			va.append(sf::Vertex(vec2, c));

			win.draw(va,di.texture.get());
			++cLine;
		}
			break;
		}
	}
}

void AptFile::UpdateTransform(PlaceObject& po, sf::Transform& di)
{

	if (po.flags.PlaceFlagHasMatrix)
	{

	}

	if (po.flags.PlaceFlagMove)
	{

	}
}

AptFile::~AptFile()
{
	if (m_aptBuf)
		delete[] m_aptBuf;
}

AptFile::AptFile() : m_frame(0), m_bgColor(55, 55, 55, 255), m_aptBuf(nullptr)
{
	
}