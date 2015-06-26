#pragma once
#include <stdint.h>
#include <sstream>
#include <vector>
#include <memory>
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

		#define CHAR_SIG 0x09876543

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

		struct OutputFrame {
			uint32_t frameitemcount;
			FrameItem **frameitems;
		};

		struct Shape : public Character {
			Rect bounds;
			uint32_t geometry;
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
			std::vector<Tri *> tris;
			uint32_t linecount;
			std::vector <Line *> lines;
			uint32_t solidstylecount;
			std::vector <SolidStyle *> solidstyles;
			uint32_t linestylecount;
			std::vector <LineStyle *> linestyles;
			uint32_t texturestylecount;
			std::vector <TextureStyle *> texturestyles;
		};

		struct Geometry {
			uint32_t entrycount;
			std::vector<GeometryEntry *> entries;
		};

		struct ParsedShape : public Character {
			Rect bounds;
			Geometry *geometry;
		};

		struct EditText : public Character {
			Rect bounds;
			uint32_t font;
			uint32_t alignment;
			uint32_t color;
			float fontheight;
			uint32_t readonly;
			uint32_t multiline;
			uint32_t wordwrap;
			const char *text;
			const char *variable;
		};

		struct OutputFont : public Character {
			const char *name;
			uint32_t glyphcount;
			uint32_t *glyphs; //offset of glyph data
		};

		struct ButtonRecord {
			uint32_t flags; //matches with ButtonFlags
			uint32_t character;
			int32_t depth;
			Transform rotateandscale;
			Vector2 translate;
			Rect color; ///always 1.0,1.0,1.0,1.0 as far as I can see
			Rect unknown; ///always zero as far as I can see
		};

		struct Triangle {
			unsigned short v1;
			unsigned short v2;
			unsigned short v3;
		};

		struct ActionString {
			uint32_t stringoffset;
			const char *string;
		};

		struct FunctionArgument {
			uint32_t reg;
			const char *name;
		};

		struct ActionDefineFunction2 {
			uint32_t definefunction2offset;
			uint32_t argumentcount;
			std::vector<FunctionArgument *> arguments;
		};

		struct ActionDefineFunction {
			uint32_t definefunctionoffset;
			uint32_t argumentcount;
			std::vector<const char *> arguments;
		};

		struct ActionPushData {
			uint32_t pushdataoffset;
			uint32_t pushdatacount;
			std::vector<uint32_t> pushdata;
		};

		struct ActionBytes {
			unsigned int actionbytecount;
			std::stringstream actionbytes;
			unsigned int constantcount;
			std::vector<unsigned int> constants;
			unsigned int stringcount;
			std::vector<ActionString *> actionstrings;
			unsigned int pushdatacount;
			std::vector<ActionPushData *> actionpushdatas;
			unsigned int definefunction2count;
			std::vector<ActionDefineFunction2 *> actiondefinefunction2s;
			unsigned int definefunctioncount;
			std::vector<ActionDefineFunction *> actiondefinefunctions;
		};

		struct Action : public FrameItem {
			ActionBytes ab;
		};

		struct OutputAction : public FrameItem {
			uint8_t *actionbytes;
		};

		struct ButtonAction {
			uint32_t flags; //matches with ButtonActionFlags;
			Action *actiondata;
		};

		struct OutputButtonAction {
			uint32_t flags; //matches with ButtonActionFlags;
			uint8_t *actiondata;
		};

		struct OutputButton : public Character {
			uint32_t unknown; //always zero as far as I can see
			Rect bounds;
			uint32_t trianglecount;
			uint32_t vertexcount;
			Vector2 *vertexes;
			Triangle *triangles;
			uint32_t recordcount;
			ButtonRecord *buttonrecords;
			uint32_t buttonactioncount;
			OutputButtonAction *buttonactionrecords;
			uint32_t unknown2; //always zero as far as I can see
		};

		struct OutputSprite : public Character {
			uint32_t framecount;
			OutputFrame *frames; //offset of frame data
			uint32_t pointer; //always zero, used at runtime
		};

		struct Image : public Character {
			uint32_t texture;
		};

		struct Texture {
			uint32_t texture;
			bool hasuv;
			uint32_t top;
			uint32_t left;
			uint32_t bottom;
			uint32_t right;
		};

		struct D3DImage : public Character {
			Texture *texture;
		};

		struct Morph : public Character {
			uint32_t startshape;
			uint32_t endshape;
		};

		struct Import {
			const char *movie;
			const char *name;
			uint32_t character;
			uint32_t pointer; //always zero, used at runtime
		};

		struct Export {
			const char *name;
			uint32_t character;
		};

		struct OutputMovie : public Character {
			uint32_t framecount;
			OutputFrame *frames; //offset of frame data
			uint32_t pointer; //always zero, used at runtime
			uint32_t charactercount;
			Character **characters; //offset of character data
			uint32_t screensizex;
			uint32_t screensizey;
			uint32_t unknown; //always 33 as far as I can see
			uint32_t importcount;
			Import *imports; //offset of imports data
			uint32_t exportcount;
			Export *exports; //offset of exports data
			uint32_t count; //always zero, used at runtime
		};

		struct Glyph {
			uint32_t index;
			int advance;
		};

		struct OutputTextRecord {
			uint32_t font;
			Rect color;
			Rect unknown; //always zero as far as I can see
			Vector2 offset;
			float textheight;
			uint32_t glyphcount;
			Glyph *glyphs;
		};

		struct OutputText : public Character {
			Rect bounds;
			Transform rotateandscale;
			Vector2 translate;
			uint32_t recordcount;
			OutputTextRecord *records;
		};

		struct FrameLabel : public FrameItem {
			const char *label;
			uint32_t flags; ///always 0x70000 as far as I can see
			uint32_t frame;
		};

		struct PlaceObjectAction {
			uint32_t flags;
			uint32_t flags2;
			Action *actiondata;
		};

		struct OutputPlaceObjectAction {
			uint32_t flags;
			uint32_t flags2;
			uint8_t *actiondata;
		};

		struct OutputPlaceObjectActions {
			uint32_t clipactioncount;
			OutputPlaceObjectAction *actions;
		};

		struct OutputPlaceObject : public FrameItem {
			uint32_t flags; //matches with PlaceObjectFlags
			int32_t depth;
			int32_t character;
			Transform rotateandscale;
			Vector2 translate;
			uint32_t colortransform;
			uint32_t unknown; ///always zero as far as I can see
			float ratio;
			const char *name;
			int32_t clipdepth;
			OutputPlaceObjectActions *poa;
		};

		struct RemoveObject : public FrameItem {
			int32_t depth;
		};

		struct BackgroundColor : public FrameItem {
			uint32_t color;
		};

		struct OutputInitAction : public FrameItem {
			uint32_t sprite;
			uint8_t *actionbytes;
		};

		enum AptConstItemType {
			TYPE_UNDEF = 0,
			TYPE_STRING = 1,
			TYPE_NUMBER = 4,
		};

		struct AptConstItem {
			AptConstItemType type;
			union {
				const char *strvalue;
				uint32_t numvalue;
			};
		};

		struct AptConstData {
			uint32_t aptdataoffset;
			uint32_t itemcount;
			std::vector<std::shared_ptr<AptConstItem>> items;
		};
#pragma endregion

	public:
		bool loadFromStream(sf::InputStream& aptStream, sf::InputStream& constStream, const std::string& name);
		void Update();
		void Render(sf::RenderWindow& win);
	private:
		std::shared_ptr<AptConstData> m_data;
		std::shared_ptr<OutputMovie> m_movie;
		std::map<uint32_t, GeometryEntry> m_geometry;
		std::map<uint32_t,uint32_t> m_dat;
		uint32_t m_frame;
	};
}