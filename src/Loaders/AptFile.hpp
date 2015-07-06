// Stephan Vedder 2015
#pragma once
#include <stdint.h>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include "BigStream.hpp"

namespace Loaders
{
	class AptFile
	{
	public:
		struct DisplayItem
		{
			uint32_t ch;
			std::shared_ptr<sf::Texture> texture;
			glm::u8vec4 color;
			glm::f32mat2 rotscale;
			glm::f32vec2 translate;
		};

		//Const File
		enum ConstItemType 
		{
			TYPE_UNDEF = 0,
			TYPE_STRING = 1,
			TYPE_NUMBER = 4,
		};

		struct ConstItem 
		{
			ConstItemType type;
			uint32_t numvalue;
			std::string strvalue;
		};

		struct ConstData 
		{
			uint32_t aptdataoffset;
			uint32_t itemcount;
			std::vector<ConstItem> items;
		};
	private:
#pragma region AptFormat
		struct Tri 
		{
			glm::f32vec2 v1;
			glm::f32vec2 v2;
			glm::f32vec2 v3;
		};

		struct Line 
		{
			glm::f32vec2 v1;
			glm::f32vec2 v2;
		};

		struct SolidStyle 
		{
			glm::u8vec4 color;
		};

		struct LineStyle 
		{
			uint32_t width;
			glm::u8vec4 color;
		};

		struct TextureStyle 
		{
			glm::u8vec4 color;
			uint32_t tex;
			glm::f32vec4 rotateandscale;
			glm::f32vec2 translate;
		};

		enum Commands
		{
			CLEAR		= 0,
			TRIANGLE	= 1,
			LINE		= 2,
			SOLIDSTYLE	= 3,
			LINESTYLE	= 4,
			TEXTURSTYLE	= 5,
		};

		enum Style
		{
			STYLE_SOLID = 0,
			STYLE_LINE = 1,
			STYLE_TEXTURE = 2,
		};

		struct GeometryEntry 
		{
			std::vector<Tri> tris;
			std::vector <Line> lines;
			std::vector <SolidStyle> solidstyles;
			std::vector <LineStyle> linestyles;
			std::vector <TextureStyle> texturestyles;
			std::vector<Commands>	order;
		};

		//Character
		enum CharacterType {
			SHAPE = 1,
			EDITTEXT = 2,
			FONT = 3,
			BUTTON = 4,
			SPRITE = 5,
			IMAGE = 7,
			MORPH = 8,
			MOVIE = 9,
			TEXT = 10
		};

		struct Character 
		{
			uint32_t type;
			uint32_t signature;
		};

		enum FrameItemType
		{
			ACTION = 1,
			FRAMELABEL = 2,
			PLACEOBJECT = 3,
			REMOVEOBJECT = 4,
			BACKGROUNDCOLOR = 5,
			INITACTION = 8
		};

		struct FrameItem
		{
			uint32_t type;
		};

		struct BackgroundColor : public FrameItem
		{
			glm::u8vec4 color;
		};

		struct Action : public FrameItem
		{
			uint8_t* bytecode;
		};

		struct FrameLabel : public FrameItem
		{
			std::string label;
			uint32_t flags;
			uint32_t frame;
		};

		struct RemoveObject : public FrameItem 
		{
			int32_t depth;
		};

		struct PlaceObjectAction 
		{
			uint64_t flags;
			uint8_t* bytecode;
		};

		struct PlaceObjectActions 
		{
			uint32_t clipactioncount;
			std::vector<PlaceObjectAction> actions;
		};

		struct PoFlags
		{
			bool PlaceFlagMove : 1;
			bool PlaceFlagHasCharacter : 1;
			bool PlaceFlagHasMatrix : 1;
			bool PlaceFlagHasColorTransform : 1;
			bool PlaceFlagHasRatio : 1;
			bool PlaceFlagHasName : 1;
			bool PlaceFlagHasClipDepth : 1;
			bool PlaceFlagHasClipActions : 1;
		};

		struct PlaceObject : public FrameItem 
		{
			PoFlags flags; //matches with PlaceObjectFlags
			int32_t depth;
			int32_t character;
			glm::f32mat2 rotateandscale;
			glm::f32vec2 translate;
			glm::u8vec4 color;
			uint32_t unknown; ///always zero as far as I can see
			float ratio;
			std::string name;
			int32_t clipdepth;
			PlaceObjectActions poa;
		};

		struct Frame 
		{
			uint32_t frameitemcount;
			std::vector<std::shared_ptr<FrameItem>> frameitems;
		};

		struct Font : public Character
		{
			std::string name;
			uint32_t glyphcount;
			std::vector<uint32_t> glyphs;
		};

		struct Shape : public Character 
		{
			glm::f32vec4 bounds;
			uint32_t geometry;
		};

		struct EditText : public Character 
		{
			glm::f32vec4 bounds;
			uint32_t font;
			uint32_t alignment;
			glm::u8vec4 color;
			float fontheight;
			uint32_t readonly;
			uint32_t multiline;
			uint32_t wordwrap;
			std::string text;
			std::string variable;
		};

		struct Sprite : public Character 
		{
			uint32_t framecount;
			std::vector<Frame> frames; //offset of frame data
			uint32_t cFrame;
			std::map<uint32_t, DisplayItem> displayList;
			std::vector<uint32_t> deleteList;
			glm::f32mat2 rotscale;
			glm::f32vec2 translate;
		};

		struct Movie : public Sprite
		{
			uint32_t framecount;
			std::vector<Frame> frames;
			uint32_t cFrame;
			uint32_t charactercount;
			std::vector<std::shared_ptr<Character>> characters;
			uint32_t width;
			uint32_t height;
		};


#pragma endregion

	private:
		GeometryEntry ParseGeometry(const std::string& name);
		std::shared_ptr<Character> ParseCharacter(uint8_t*& buffer, uint8_t* base);
		std::shared_ptr<FrameItem> ParseFrameItem(uint8_t*& buffer, uint8_t* base);

		void UpdateFrame(Frame& frame, DisplayItem& parent);
		void UpdateTransform(PlaceObject& po, sf::Transform& t);

		void RenderGeometry(sf::RenderWindow& win, DisplayItem& di, glm::f32vec2 offset, glm::f32mat2 rotscale);
		void Render(sf::RenderWindow& win, std::map<uint32_t, DisplayItem>& displaylist,glm::f32vec2 offset,glm::f32mat2 rotscale);

		void Update(std::map<uint32_t, DisplayItem>& displaylist, std::vector<uint32_t>& deleteList);
	public:

		bool loadFromStream(sf::InputStream& aptStream, sf::InputStream& constStream, const std::string& name);
		inline void Update()
		{
			Update(m_displaylist, m_deleteList);
		}

		inline void Render(sf::RenderWindow& win)
		{
			win.clear(m_bgColor);
			Render(win, m_displaylist,glm::f32vec2(),glm::f32mat2());
		}
		
		~AptFile();
		AptFile();
	private:
		std::string m_name;
		ConstData m_data;
		Movie m_movie;
		std::map<uint32_t, GeometryEntry> m_geometry;
		std::map<uint32_t, uint32_t> m_dat;
		std::map<uint32_t, std::shared_ptr<sf::Texture>> m_textures;
		std::map<uint32_t, std::shared_ptr<Character>> m_characters;
		std::map<uint32_t, DisplayItem> m_displaylist;
		std::vector<uint32_t> m_deleteList;
		uint32_t m_frame;
		uint8_t* m_aptBuf;
		sf::Color m_bgColor;
	};
}