#include "AptFile.hpp"
#include "Util.hpp"
#include "BigStream.hpp"
#include <iostream>

using namespace Loaders;
using namespace Util;

#define STRLENGTH(x) (4 * ((((uint32_t)strlen(x) + 1) + 3)/4))
#define GETALIGN(x) ((4 * ((x + 3) / 4)) - x)
#define ALIGN(x) x = ((uint8_t *)(4 * ((((uintptr_t)x) + 3) / 4)))
#define B(x) x?"true":"false"
#define add(x) *((uint8_t **)&x) += (uintptr_t)aptBuf; 

bool AptFile::loadFromStream(sf::InputStream& aptStream, sf::InputStream& constStream,const std::string& name)
{
	uint8_t* aptBuf = new uint8_t[aptStream.getSize()];
	uint8_t* constBuf = new uint8_t[constStream.getSize()];
	aptStream.read(aptBuf, aptStream.getSize());
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

	m_movie = std::shared_ptr<OutputMovie>((OutputMovie *)(aptBuf + m_data->aptdataoffset));
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
	std::cout << "Parsed " << name << ".apt";
	delete[] aptBuf;

	BigStream datStream;
	if(!datStream.open(name+".dat"))
	{
		return true;
	}
	std::cout << "Parsed " << name << ".dat";
	return true;
}

void AptFile::Update()
{
	++m_frame;

}

void Render(sf::RenderWindow& win)
{

}