// Stephan Vedder 2015
#include "ActionScript.hpp"
#include "../Loaders/Util.hpp"
#include <stack>
using namespace Script;
#define STRLENGTH(x) (4 * ((((uint32_t)strlen(x) + 1) + 3)/4))
#define GETALIGN(x) ((4 * ((x + 3) / 4)) - x)
#define ALIGN(x) x = ((uint8_t *)(4 * ((((uintptr_t)x) + 3) / 4)))
#define add(x) *((uint8_t **)&x) += (uintptr_t)apt; 

uint32_t AS::GetBytecodeSize(uint8_t* a)
{
	uint8_t* base = a;
	ActionCode action;
	do
	{
		action = (ActionCode)(*a);
		a++;
		switch (action)
		{
		case ACTION_BRANCHALWAYS:
		case ACTION_BRANCHIFTRUE:
		case EA_BRANCHIFFALSE:
		case ACTION_GOTOFRAME:
		case ACTION_SETREGISTER:
		case ACTION_WITH:
		case ACTION_GOTOEXPRESSION:
		{
			ALIGN(a);
			a += 4;
		}
			break;
		case ACTION_GETURL:
		case ACTION_PUSHDATA:
		case ACTION_CONSTANTPOOL:
		{
			ALIGN(a);
			a += 8;
		}
			break;
		case ACTION_DEFINEFUNCTION2:
		{
			ALIGN(a);
			a += 28;
		}
			break;
		case ACTION_DEFINEFUNCTION:
		{
			ALIGN(a);
			a += 24;
		}
			break;
		case EA_PUSHSTRING:
		case EA_GETSTRINGVAR:
		case EA_GETSTRINGMEMBER:
		case EA_SETSTRINGVAR:
		case EA_SETSTRINGMEMBER:
		case ACTION_SETTARGET:
		case ACTION_GOTOLABEL:
		{
			ALIGN(a);
			a += 4;
		}
			break;
		case EA_CALLNAMEDFUNCTIONPOP:
		case EA_CALLNAMEDFUNCTION:
		case EA_CALLNAMEDMETHODPOP:
		case EA_CALLNAMEDMETHOD:
		case EA_PUSHCONSTANT:
		case EA_PUSHVALUEOFVAR:
		case EA_GETNAMEDMEMBER:
		case EA_PUSHBYTE:
		case EA_PUSHREGISTER:
		{
			a += 1;
		}
			break;
		case EA_PUSHWORDCONSTANT:
		case EA_PUSHSHORT:
		{
			a += 2;
		}
			break;
		case EA_PUSHFLOAT:
		case EA_PUSHLONG:
		{
			a += 4;
		}
			break;
		}
	} while (action != ACTION_END);

	uint32_t size = a-base;

	return size;
}

bool AS::ExecuteBytecode(uint8_t* a, Loaders::AptFile::DisplayItem& di, Loaders::AptFile::ConstData& data, uint8_t* apt)
{
	uint8_t* base = a;
	std::stack<StackValue> stack;
	std::vector<Loaders::AptFile::ConstItem> entries;
	std::map<std::string, Function> funcs;
	ActionCode action;
	do
	{
		action = (ActionCode)(*a);
		a++;
		switch (action)
		{
		case ACTION_BRANCHALWAYS:
		case ACTION_BRANCHIFTRUE:
		case EA_BRANCHIFFALSE:
		case ACTION_GOTOFRAME:
		case ACTION_SETREGISTER:
		case ACTION_WITH:
		case ACTION_GOTOEXPRESSION:
		{
			ALIGN(a);
			a += 4;
		}
			break;
		case ACTION_GETURL:
		case ACTION_PUSHDATA:
		{
			ALIGN(a);
			a += 8;
		}
			break;
		case ACTION_CONSTANTPOOL:
		{
			ALIGN(a);
			auto count = Util::Read<uint32_t>(a);
			auto cpd = reinterpret_cast<uint32_t*>(Util::Read<uint32_t>(a)+apt);
			
			for (uint32_t i = 0; i < count; ++i)
			{
				auto id = data.items[cpd[i]];
				entries.push_back(id);
			}
		}
			break;
		case ACTION_DEFINEFUNCTION2:
		{
			ALIGN(a);
			a += 28;
		}
			break;
		case ACTION_DEFINEFUNCTION:
		{
			ALIGN(a);
			auto offset = apt + Util::Read<uint32_t>(a);
			Function func;
			func.name =  std::string(reinterpret_cast<const char*>(offset));
			func.argCount = Util::Read<uint32_t>(a);
			add(*(uint32_t *)a);
			char **args = (char **)(*(uint32_t *)a);
			a += 4;

			for (uint32_t i = 0; i < func.argCount; i++)
			{
				add(args[i]);
				func.argNames.push_back(args[i]);
			}
			func.size = Util::Read<uint32_t>(a);
			func.body = a;
			a += func.size;
			a += 8;
			funcs[func.name] = func;
		}
			break;
		case EA_PUSHSTRING:
		case EA_GETSTRINGVAR:
		case EA_GETSTRINGMEMBER:
		case EA_SETSTRINGVAR:
		case EA_SETSTRINGMEMBER:
		case ACTION_SETTARGET:
		case ACTION_GOTOLABEL:
		{
			ALIGN(a);
			a += 4;
		}
			break;
		case EA_CALLNAMEDFUNCTIONPOP:
		case EA_CALLNAMEDFUNCTION:
		case EA_CALLNAMEDMETHODPOP:
		case EA_CALLNAMEDMETHOD:		
		case EA_PUSHBYTE:
		case EA_PUSHREGISTER:
		{
			a += 1;
		}
			break;
		case EA_GETNAMEDMEMBER:
		case EA_PUSHVALUEOFVAR:
		case EA_PUSHCONSTANT:
		{
			auto id = Util::Read<uint8_t>(a);
			auto item = entries[id];
			StackValue val;
			if (item.type == Loaders::AptFile::TYPE_NUMBER)
			{
				val.type = INTEGER;
				val.integerVal = item.numvalue;
			}
			else if (item.type == Loaders::AptFile::TYPE_STRING)
			{
				val.type = STRING;
				val.stringVal = item.strvalue;
			}

			stack.push(val);
		}
			break;
		case EA_PUSHWORDCONSTANT:
		case EA_PUSHSHORT:
		{
			a += 2;
		}
			break;
		case EA_PUSHFLOAT:
		case EA_PUSHLONG:
		{
			a += 4;
		}
			break;
		}
	} while (action != ACTION_END);

	uint32_t size = a - base;

	return true;
}