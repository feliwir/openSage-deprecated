#include <iostream>
#include <string>
#include "AptFile.hpp"
#include "Util.hpp"
#include "BigStream.hpp"
#include "../Game/INI.hpp"

using namespace Loaders;
using namespace Util;

#define STRLENGTH(x) (4 * ((((uint32_t)strlen(x) + 1) + 3)/4))
#define GETALIGN(x) ((4 * ((x + 3) / 4)) - x)
#define ALIGN(x) x = ((uint8_t *)(4 * ((((uintptr_t)x) + 3) / 4)))
#define add(x) *((uint8_t **)&x) += (uintptr_t)m_aptBuf; 

bool AptFile::loadFromStream(sf::InputStream& aptStream, sf::InputStream& constStream,const std::string& name)
{
	m_aptBuf = new uint8_t[aptStream.getSize()];
	uint8_t* constBuf = new uint8_t[constStream.getSize()];
	aptStream.read(m_aptBuf, aptStream.getSize());
	constStream.read(constBuf, constStream.getSize());

	m_data = std::make_shared<AptConstData>();

	//now start parsing by setting our iter to the start of the aptbuffer
	uint8_t* iter = constBuf;
	//skip the first 20 bytes
	iter += 0x14;
	m_data->aptdataoffset = Read<uint32_t>(iter);
	m_data->itemcount = Read<uint32_t>(iter);
	//skip 4 bytes now
	iter += 4;

	AptConstItem *aci = (AptConstItem *)iter;
	//8 bytes per const item
	iter += m_data->itemcount * 8;
	//now read each item
	for (uint32_t i = 0; i < m_data->itemcount; ++i)
	{
		m_data->items.push_back(std::make_shared<AptConstItem>());
		m_data->items[i]->type = aci->type;
		if (m_data->items[i]->type == TYPE_STRING)
		{
			//read the string from the position specified
			m_data->items[i]->strvalue = (char *)(constBuf + (uintptr_t)aci->strvalue);
		}
		else
		{
			m_data->items[i]->numvalue = aci->numvalue;
		}
		aci++;
	}

	std::cout << "Parsed " << name << ".const";
	delete[] constBuf;

	m_movie = std::shared_ptr<OutputMovie>((OutputMovie *)(m_aptBuf + m_data->aptdataoffset));
	add(m_movie->characters);
	add(m_movie->exports);
	add(m_movie->imports);
	add(m_movie->frames);

	for (uint32_t i = 0; i < m_movie->importcount; i++)
	{
		add(m_movie->imports[i].movie);
		add(m_movie->imports[i].name);
	}

	for (uint32_t i = 0; i < m_movie->exportcount; i++)
	{
		add(m_movie->exports[i].name);
	}

	for (uint32_t i = 0; i < m_movie->framecount; i++)
	{
		add(m_movie->frames[i].frameitems);

		for (uint32_t j = 0; j < m_movie->frames[i].frameitemcount; j++)
		{
			add(m_movie->frames[i].frameitems[j]);
			switch (m_movie->frames[i].frameitems[j]->type)
			{
			case ACTION:
			{
				OutputAction *oa = (OutputAction *)m_movie->frames[i].frameitems[j];
				add(oa->actionbytes);
			}
				break;
			case FRAMELABEL:
			{
				FrameLabel *fl = (FrameLabel *)m_movie->frames[i].frameitems[j];
				add(fl->label);
			}
				break;
			case PLACEOBJECT:
			{
				OutputPlaceObject *po = (OutputPlaceObject *)m_movie->frames[i].frameitems[j];
				//set all attributes of this node

				if (po->name)
				{
					add(po->name);
				}
				if (po->poa)
				{
					add(po->poa);
					add(po->poa->actions);

					for (uint32_t k = 0; k < po->poa->clipactioncount; k++)
					{
						add(po->poa->actions[k].actiondata);
					}
				}
			}
				break;
			case INITACTION:
			{
				OutputInitAction *oa = (OutputInitAction *)m_movie->frames[i].frameitems[j];
				add(oa->actionbytes);
			}
				break;
			}
		}
	}

	for (uint32_t ch = 0; ch < m_movie->charactercount; ch++)
	{
		if (m_movie->characters[ch])
		{
			add(m_movie->characters[ch]);
			switch (m_movie->characters[ch]->type)
			{
			case SHAPE:
			{
				Shape *sh = (Shape *)m_movie->characters[ch];
				std::string runame = name + "_geometry/" + std::to_string(sh->geometry) + ".ru";
				m_geometry[sh->geometry] = ParseGeometry(runame);
				
			}
				break;
			case EDITTEXT:
			{
				EditText *et = (EditText *)m_movie->characters[ch];
							
				if (et->text)
				{
					add(et->text);
				}
				if (et->variable)
				{
					add(et->variable);
				}
			}
				break;
			case FONT:
			{
				OutputFont *fo = (OutputFont *)m_movie->characters[ch];
				add(fo->name);

				if (fo->glyphcount)
				{
					add(fo->glyphs);
				}
			}
				break;
			case BUTTON:
			{
				OutputButton *ob = (OutputButton *)m_movie->characters[ch];
				if (ob->trianglecount)
				{
					add(ob->vertexes);
					add(ob->triangles);							 
				}
				if (ob->recordcount)
				{
					add(ob->buttonrecords);
				}
				if (ob->buttonactioncount)
				{
					add(ob->buttonactionrecords);

					for (uint32_t i = 0; i < ob->buttonactioncount; i++)
					{
						add(ob->buttonactionrecords[i].actiondata);
					}
				}
			}
				break;
			case SPRITE:
			{
				OutputSprite *sp = (OutputSprite *)m_movie->characters[ch];

				add(sp->frames);
				if (sp->framecount)
				{
					for (uint32_t i = 0; i < sp->framecount; i++)
					{
						add(sp->frames[i].frameitems);

						for (uint32_t j = 0; j < sp->frames[i].frameitemcount; j++)
						{
							add(sp->frames[i].frameitems[j]);
							switch (sp->frames[i].frameitems[j]->type)
							{
								case ACTION:
								{
									OutputAction *oa = (OutputAction *)sp->frames[i].frameitems[j];
									add(oa->actionbytes);
								}
									break;
								case FRAMELABEL:
								{
									FrameLabel *fl = (FrameLabel *)sp->frames[i].frameitems[j];
									add(fl->label);
								}
									break;
								case PLACEOBJECT:
								{
									OutputPlaceObject *po = (OutputPlaceObject *)sp->frames[i].frameitems[j];

									if (po->name)
									{
										add(po->name);
									}
									if (po->poa)
									{
										add(po->poa);
										add(po->poa->actions);

										for (uint32_t k = 0; k < po->poa->clipactioncount; k++)
										{
											add(po->poa->actions[k].actiondata);

										}
									}
								}
									break;
								case INITACTION:
								{
									OutputInitAction *oa = (OutputInitAction *)sp->frames[i].frameitems[j];
									add(oa->actionbytes);
								}
									break;
							}
						}
					}
				}
			}
				break;
			case TEXT:
			{
				OutputText *te = (OutputText *)m_movie->characters[ch];

				if (te->recordcount)
				{
					add(te->records);
					for (uint32_t i = 0; i < te->recordcount; i++)
					{
						if (te->records[i].glyphcount)
						{
							add(te->records[i].glyphs);
						}
					}
				}
			}
				break;
			case IMAGE:
			{
				Image *img = (Image *)m_movie->characters[ch];
			}
				break;
			default:
				break;
			}
		}
	}
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

void AptFile::Update()
{
	uint32_t currentFrame = m_frame % m_movie->framecount;

	for (uint32_t i = 0; i < m_movie->frames[currentFrame].frameitemcount;++i)
	{
		switch (m_movie->frames[currentFrame].frameitems[i]->type)
		{
		case PLACEOBJECT:
		{
			auto po = static_cast<OutputPlaceObject*>(m_movie->frames[currentFrame].frameitems[i]);
			m_objects[po->depth] = po;
		}
			break;
		case REMOVEOBJECT:
		{
			auto ro = static_cast<RemoveObject*>(m_movie->frames[currentFrame].frameitems[i]);
			m_objects.erase(ro->depth);
		}
			break;
		case BACKGROUNDCOLOR:
		{
			auto bg = static_cast<BackgroundColor*>(m_movie->frames[currentFrame].frameitems[i]);
			m_bgColor = sf::Color(bg->red, bg->green, bg->blue, bg->alpha);
		}
			break;
		case FRAMELABEL:
		{
			auto fl = static_cast<FrameLabel*>(m_movie->frames[currentFrame].frameitems[i]);
			std::cout << "Framelabel: " << fl->label << std::endl;
		}
			break;
		}
	}

	++m_frame;
}

void AptFile::Render(sf::RenderWindow& win)
{
	win.clear(m_bgColor);

	for (auto& o : m_objects)
	{
		auto po = o.second;
		auto ch = m_movie->characters[po->character];

		switch (ch->type)
		{
		case SHAPE:
		{
			auto shape = static_cast<Shape*>(ch);
			auto geo = shape->geometry;
		}
			break;
		case SPRITE:
		{
			auto sprite = static_cast<OutputSprite*>(ch);
		}
			break;
		default:
			break;
		}
	}
}

AptFile::~AptFile()
{
	if (m_aptBuf)
		delete[] m_aptBuf;
}

AptFile::AptFile() : m_aptBuf(nullptr), m_frame(0)
{
}