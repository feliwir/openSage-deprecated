// Stephan Vedder 2015
#pragma once
#include <stdint.h>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <glm/glm.hpp>

namespace Loaders
{
    class AptFile;
}

namespace Script
{
	class AS
	{
	private:
		enum ActionCode
		{
			ACTION_END = 0x00,
			ACTION_NEXTFRAME = 0x04,
			ACTION_PREVFRAME = 0x05,
			ACTION_PLAY = 0x06,
			ACTION_STOP = 0x07,
			ACTION_TOGGLEQUALITY = 0x08,
			ACTION_STOPSOUNDS = 0x09,
			ACTION_GOTOFRAME = 0x81,
			ACTION_GETURL = 0x83,
			ACTION_WAITFORFRAME = 0x8A,
			ACTION_SETTARGET = 0x8B,
			ACTION_GOTOLABEL = 0x8C,
			ACTION_ADD = 0x0A,
			ACTION_SUBTRACT = 0x0B,
			ACTION_MULTIPLY = 0x0C,
			ACTION_DIVIDE = 0x0D,
			ACTION_EQUAL = 0x0E,
			ACTION_LESSTHAN = 0x0F,
			ACTION_LOGICALAND = 0x10,
			ACTION_LOGICALOR = 0x11,
			ACTION_LOGICALNOT = 0x12,
			ACTION_STRINGEQ = 0x13,
			ACTION_STRINGLENGTH = 0x14,
			ACTION_SUBSTRING = 0x15,
			ACTION_POP = 0x17,
			ACTION_INT = 0x18,
			ACTION_GETVARIABLE = 0x1C,
			ACTION_SETVARIABLE = 0x1D,
			ACTION_SETTARGETEXPRESSION = 0x20,
			ACTION_STRINGCONCAT = 0x21,
			ACTION_GETPROPERTY = 0x22,
			ACTION_SETPROPERTY = 0x23,
			ACTION_DUPLICATECLIP = 0x24,
			ACTION_REMOVECLIP = 0x25,
			ACTION_TRACE = 0x26,
			ACTION_STARTDRAGMOVIE = 0x27,
			ACTION_STOPDRAGMOVIE = 0x28,
			ACTION_STRINGCOMPARE = 0x29,
			ACTION_THROW = 0x2A,
			ACTION_CASTOP = 0x2B,
			ACTION_IMPLEMENTSOP = 0x2C,
			ACTION_RANDOM = 0x30,
			ACTION_MBLENGTH = 0x31,
			ACTION_ORD = 0x32,
			ACTION_CHR = 0x33,
			ACTION_GETTIMER = 0x34,
			ACTION_MBSUBSTRING = 0x35,
			ACTION_MBORD = 0x36,
			ACTION_MBCHR = 0x37,
			ACTION_WAITFORFRAMEEXPRESSION = 0x8D,
			ACTION_PUSHDATA = 0x96,
			ACTION_BRANCHALWAYS = 0x99,
			ACTION_GETURL2 = 0x9A,
			ACTION_BRANCHIFTRUE = 0x9D,
			ACTION_CALLFRAME = 0x9E,
			ACTION_GOTOEXPRESSION = 0x9F,
			ACTION_DELETE = 0x3A,
			ACTION_DELETE2 = 0x3B,
			ACTION_DEFINELOCAL = 0x3C,
			ACTION_CALLFUNCTION = 0x3D,
			ACTION_RETURN = 0x3E,
			ACTION_MODULO = 0x3F,
			ACTION_NEW = 0x40,
			ACTION_VAR = 0x41,
			ACTION_INITARRAY = 0x42,
			ACTION_INITOBJECT = 0x43,
			ACTION_TYPEOF = 0x44,
			ACTION_TARGETPATH = 0x45,
			ACTION_ENUMERATE = 0x46,
			ACTION_NEWADD = 0x47,
			ACTION_NEWLESSTHAN = 0x48,
			ACTION_NEWEQUALS = 0x49,
			ACTION_TONUMBER = 0x4A,
			ACTION_TOSTRING = 0x4B,
			ACTION_DUP = 0x4C,
			ACTION_SWAP = 0x4D,
			ACTION_GETMEMBER = 0x4E,
			ACTION_SETMEMBER = 0x4F,
			ACTION_INCREMENT = 0x50,
			ACTION_DECREMENT = 0x51,
			ACTION_CALLMETHOD = 0x52,
			ACTION_NEWMETHOD = 0x53,
			ACTION_INSTANCEOF = 0x54,
			ACTION_ENUM2 = 0x55,
			ACTION_BITWISEAND = 0x60,
			ACTION_BITWISEOR = 0x61,
			ACTION_BITWISEXOR = 0x62,
			ACTION_SHIFTLEFT = 0x63,
			ACTION_SHIFTRIGHT = 0x64,
			ACTION_SHIFTRIGHT2 = 0x65,
			ACTION_STRICTEQ = 0x66,
			ACTION_GREATER = 0x67,
			ACTION_STRINGGREATER = 0x68,
			ACTION_EXTENDS = 0x69,
			ACTION_CONSTANTPOOL = 0x88,
			ACTION_DEFINEFUNCTION2 = 0x8E,
			ACTION_TRY = 0x8F,
			ACTION_WITH = 0x94,
			ACTION_DEFINEFUNCTION = 0x9B,
			ACTION_SETREGISTER = 0x87,
			EA_ACTION56 = 0x56,
			EA_ACTION58 = 0x58,
			EA_PUSHZERO = 0x59,
			EA_PUSHONE = 0x5A,
			EA_CALLFUNCTIONPOP = 0x5B,
			EA_CALLFUNCTION = 0x5C,
			EA_CALLMETHODPOP = 0x5D,
			EA_CALLMETHOD = 0x5E,
			EA_PUSHTHIS = 0x70,
			EA_PUSHGLOBAL = 0x71,
			EA_ZEROVARIABLE = 0x72,
			EA_PUSHTRUE = 0x73,
			EA_PUSHFALSE = 0x74,
			EA_PUSHNULL = 0x75,
			EA_PUSHUNDEFINED = 0x76,
			EA_ACTION77 = 0x77,
			EA_PUSHSTRING = 0xA1,
			EA_PUSHCONSTANT = 0xA2,
			EA_PUSHWORDCONSTANT = 0xA3,
			EA_GETSTRINGVAR = 0xA4,
			EA_GETSTRINGMEMBER = 0xA5,
			EA_SETSTRINGVAR = 0xA6,
			EA_SETSTRINGMEMBER = 0xA7,
			EA_PUSHVALUEOFVAR = 0xAE,
			EA_GETNAMEDMEMBER = 0xAF,
			EA_CALLNAMEDFUNCTIONPOP = 0xB0,
			EA_CALLNAMEDFUNCTION = 0xB1,
			EA_CALLNAMEDMETHODPOP = 0xB2,
			EA_CALLNAMEDMETHOD = 0xB3,
			EA_PUSHFLOAT = 0xB4,
			EA_PUSHBYTE = 0xB5,
			EA_PUSHSHORT = 0xB6,
			EA_PUSHLONG = 0xB7,
			EA_BRANCHIFFALSE = 0xB8,
			EA_PUSHREGISTER = 0xB9
		};
	
		struct Function
		{
			std::string name;
			uint32_t argCount;
			std::vector<std::string> argNames;
			uint32_t size;
			uint8_t* body;
		};

    public:
		enum ValueType
		{
            UNDEFINED   = 0,
			CONSTANT	= 1,
			STRING		= 2,
			INTEGER		= 3,
			LONG		= 4,
			FLOAT		= 5
		};

		struct Value
		{
			ValueType type;
			uint32_t constVal;
			std::string stringVal;
			uint32_t integerVal;
		};

        struct Context
        {
            std::map<std::string, Value> vars;
        };

	public:
		static uint32_t getBytecodeSize(uint8_t* bytecode);
		static bool executeBytecode(uint8_t* bytecode, Loaders::AptFile* apt);
	};

}
