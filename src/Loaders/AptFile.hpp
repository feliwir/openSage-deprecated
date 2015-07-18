// Stephan Vedder 2015
#pragma once
#include <stdint.h>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <SFML/System.hpp>
#include <glm/glm.hpp>
#include "BigStream.hpp"
#include "../Script/ActionScript.hpp"

namespace Loaders
{
	class AptFile
	{
	public:
		struct Object
		{
			uint32_t ch;
			std::shared_ptr<sf::Texture> texture;
			glm::u8vec4 color;
			glm::f32mat2 rotscale;
			glm::f32vec2 translate;
            std::string name;
            std::map<std::string, Script::AS::Value> members;
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
            std::shared_ptr<sf::Texture> tex;
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
			std::vector<Line> lines;
			std::vector<SolidStyle> solidstyles;
			std::vector<LineStyle> linestyles;
			std::vector<TextureStyle> texturestyles;
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
            GeometryEntry data;
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
            std::map<uint32_t, Object> displayList;
			std::vector<uint32_t> deleteList;
			glm::f32mat2 rotscale;
			glm::f32vec2 translate;
		};

        struct Import
        {
            std::string movie;
            std::string name;
            uint32_t character;
            uint32_t pointer; //always zero, used at runtime
        };

        struct Export
        {
            std::string name;
            uint32_t character;
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
            uint32_t unknown;
            uint32_t importcount;
            std::vector<Import> imports;
            uint32_t exportcount;
            std::vector<Export> exports;
		};

#pragma endregion

	private:
		GeometryEntry ParseGeometry(const std::string& name);
		std::shared_ptr<Character> ParseCharacter(uint8_t*& buffer, uint8_t* base);
		std::shared_ptr<FrameItem> ParseFrameItem(uint8_t*& buffer, uint8_t* base);

        void UpdateFrame(Frame& frame, Object& parent);

        void RenderGeometry(sf::Window& win, Object& di, glm::f32vec2 offset, glm::f32mat2 rotscale);
        void Render(sf::Window& win, std::map<uint32_t, Object>& displaylist, glm::f32vec2 offset, glm::f32mat2 rotscale);

        void Update(std::map<uint32_t, Object>& displaylist, std::vector<uint32_t>& deleteList);
	public:
        bool loadFromStream(std::shared_ptr<sf::InputStream> aptStream, std::shared_ptr<sf::InputStream> constStream, const std::string& name);

		inline void Update()
		{
			Update(m_displaylist, m_deleteList);
		}

        inline std::shared_ptr<Character> GetCharacter(const uint32_t id)
        {
            return m_characters[id];
        }

        inline Object& GetObject(const std::string& name)
        {
            for (auto& o : m_displaylist)
            {
                if (o.second.name == name)
                    return o.second;
            }

            return Object();
        }

        inline ConstData& GetConstData()
        {
            return m_data;
        }

        inline const uint8_t* GetMemory()
        {
            return m_aptBuf;
        }

        inline Script::AS::Context& GetContext()
        {
            return m_asCtx;
        }

        inline std::shared_ptr<Character> GetExport(const std::string& name)
        {
            auto movie = std::static_pointer_cast<Movie>(m_characters[0]);
            for (auto& e : movie->exports)
            {
                if (e.name == name)
                    return m_characters[e.character];
            }
        }

		~AptFile();
		AptFile();
	private:
		std::string m_name;
		ConstData m_data;
		Movie m_movie;
		std::map<uint32_t, uint32_t> m_dat;
		std::map<uint32_t, std::shared_ptr<Character>> m_characters;
		std::map<uint32_t, Object> m_displaylist;
		std::vector<uint32_t> m_deleteList;
        Script::AS::Context m_asCtx;
		uint32_t m_frame;
		uint8_t* m_aptBuf;
        static std::map<std::string, std::shared_ptr<AptFile>> aptfiles;
	};
}