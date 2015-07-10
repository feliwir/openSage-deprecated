// Stephan Vedder 2015
#include "ActionScript.hpp"
#include "../Loaders/Util.hpp"
#include "../Loaders/AptFile.hpp"
#include <stack>

using namespace Script;

#define STRLENGTH(x) (4 * ((((uint32_t)strlen(x) + 1) + 3)/4))
#define GETALIGN(x) ((4 * ((x + 3) / 4)) - x)
#define ALIGN(x) x = ((uint8_t *)(4 * ((((uintptr_t)x) + 3) / 4)))
#define add(x) *((uint8_t **)&x) += (uintptr_t)apt->GetMemory(); 

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

bool AS::ExecuteBytecode(uint8_t* a, Loaders::AptFile* apt)
{
	uint8_t* base = a;
    std::stack<Value> stack;
	std::vector<Loaders::AptFile::ConstItem> cpool;
	std::map<std::string, Function> funcs;
	ActionCode action;
	do
	{
		action = (ActionCode)(*a);
		a++;
		switch (action)
		{
        case ACTION_SETMEMBER:
        {
            auto valStr = stack.top();
            stack.pop();
            auto nameStr = stack.top();
            stack.pop();
            auto objectStr = stack.top();
            stack.pop();
            auto object = apt->GetObject(objectStr.stringVal);
            object.members[nameStr.stringVal] = valStr;
        }
            break;
        case ACTION_GETVARIABLE:
        {

        }
            break;
        case ACTION_SETVARIABLE:
        {
            auto val = stack.top();
            stack.pop();
            auto name = stack.top();
            stack.pop();
            apt->GetContext().vars[name.stringVal] = val;
        }
            break;
        case ACTION_BRANCHIFTRUE:
        {
            ALIGN(a);
            auto cond = stack.top();
            stack.pop();
            if (cond.integerVal)
                a += Util::Read<int32_t>(a);
        }
		case ACTION_BRANCHALWAYS:
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
		{
			ALIGN(a);
			a += 8;
		}
			break;
		case ACTION_PUSHDATA:
		{
			ALIGN(a);
			auto count = Util::Read<uint32_t>(a);
			add(*(uint32_t *)a);
			uint32_t *pid = *(uint32_t **)a;
			a += 4;

			for (uint32_t i = 0; i < count; ++i)
			{
				auto item = apt->GetConstData().items[pid[i]];
                Value val;
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
		}
			break;
		case ACTION_CONSTANTPOOL:
		{
			ALIGN(a);
			auto count = Util::Read<uint32_t>(a);
			auto cpd = reinterpret_cast<const uint32_t*>(Util::Read<uint32_t>(a)+apt->GetMemory());
			
			for (uint32_t i = 0; i < count; ++i)
			{
                auto item = apt->GetConstData().items[cpd[i]];
                cpool.push_back(item);
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
			auto offset = apt->GetMemory() + Util::Read<uint32_t>(a);
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
        case ACTION_LOGICALNOT:
        {
            auto val = stack.top();
            stack.pop();
            val.integerVal = !val.integerVal;
            stack.push(val);
        }
            break;
        case EA_PUSHVALUEOFVAR:
		case EA_PUSHCONSTANT:
		{
			auto id = Util::Read<uint8_t>(a);
            auto item = cpool[id];
			Value val;
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
        case EA_GETNAMEDMEMBER:
        {

            auto id = Util::Read<uint8_t>(a);
            auto item = cpool[id];
            Value member;
            if (item.type == Loaders::AptFile::TYPE_NUMBER)
            {
                member.type = INTEGER;
                member.integerVal = item.numvalue;
            }
            else if (item.type == Loaders::AptFile::TYPE_STRING)
            {
                member.type = STRING;
                member.stringVal = item.strvalue;
            }

            auto objectStr = stack.top();
            stack.pop();
            auto object = apt->GetObject(objectStr.stringVal);
            stack.push(object.members[member.stringVal]);
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