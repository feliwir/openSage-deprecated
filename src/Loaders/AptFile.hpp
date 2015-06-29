#pragma once
#include <stdint.h>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "BigStream.hpp"


namespace Loaders
{
	class AptFile
	{
	private:
#pragma region AptFormat

		struct Vector2 {
			float X;
			float Y;
		};

		struct Transform {
			float m00;
			float m01;
			float m10;
			float m11;
		};

		struct Rect
		{
			float left;
			float top;
			float right;
			float bottom;
		};

		struct Tri {
			Vector2 v1;
			Vector2 v2;
			Vector2 v3;
		};

		struct Line {
			Vector2 v1;
			Vector2 v2;
		};

		struct SolidStyle {
			uint32_t red;
			uint32_t green;
			uint32_t blue;
			uint32_t alpha;
		};

		struct LineStyle {
			uint32_t width;
			uint32_t red;
			uint32_t green;
			uint32_t blue;
			uint32_t alpha;
		};

		struct TextureStyle {
			uint32_t red;
			uint32_t green;
			uint32_t blue;
			uint32_t alpha;
			uint32_t TextureCharacter;
			Transform rotateandscale;
			Vector2 translate;
		};

		struct GeometryEntry {
			uint32_t tricount;
			std::vector<Tri> tris;
			uint32_t linecount;
			std::vector <Line> lines;
			uint32_t solidstylecount;
			std::vector <SolidStyle> solidstyles;
			uint32_t linestylecount;
			std::vector <LineStyle> linestyles;
			uint32_t texturestylecount;
			std::vector <TextureStyle> texturestyles;
		};

		struct Triangle {
			unsigned short v1;
			unsigned short v2;
			unsigned short v3;
		};

		//Const File
		enum ConstItemType {
			TYPE_UNDEF = 0,
			TYPE_STRING = 1,
			TYPE_NUMBER = 4,
		};

		struct ConstItem {
			ConstItemType type;
			union {
				const char *strvalue;
				uint32_t numvalue;
			};
		};

		struct ConstData {
			uint32_t aptdataoffset;
			uint32_t itemcount;
			std::vector<ConstItem> items;
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

		struct Character {
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
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;
		};

		struct Action : public FrameItem
		{
			uint8_t* bytecode;
		};

		struct PlaceObjectAction {
			uint64_t flags;
			uint8_t* bytecode;
		};

		struct PlaceObjectActions {
			uint32_t clipactioncount;
			std::vector<PlaceObjectAction> actions;
		};

		struct PlaceObject : public FrameItem {
			uint32_t flags; //matches with PlaceObjectFlags
			int32_t depth;
			int32_t character;
			Transform rotateandscale;
			Vector2 translate;
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;
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

		struct Shape : public Character
		{
			Rect bounds;
			uint32_t geometry;
		};

		struct Movie : public Character
		{
			uint32_t framecount;
			std::vector<Frame> frames;
		};
#pragma endregion

	private:
		GeometryEntry ParseGeometry(const std::string& name);
		std::shared_ptr<Character> ParseCharacter(uint8_t*& buffer, uint8_t* base);
		std::shared_ptr<FrameItem> ParseFrameItem(uint8_t*& buffer, uint8_t* base);
		uint32_t GetBytecodeSize(uint8_t* bytecode);
	public:
		bool loadFromStream(sf::InputStream& aptStream, sf::InputStream& constStream, const std::string& name);
		void Update();
		void Render(sf::RenderWindow& win);
		~AptFile();
		AptFile();
	private:
		ConstData m_data;
		Movie m_movie;
		std::map<uint32_t, GeometryEntry> m_geometry;
		std::map<uint32_t, uint32_t> m_dat;
		std::map<uint32_t, std::shared_ptr<sf::Texture>> m_textures;
		std::map<uint32_t, std::shared_ptr<Character>> m_characters;
		uint32_t m_frame;
		sf::Color m_bgColor;
	};
}