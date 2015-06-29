#include "ActionScript.hpp"

using namespace Script;
#define STRLENGTH(x) (4 * ((((uint32_t)strlen(x) + 1) + 3)/4))
#define GETALIGN(x) ((4 * ((x + 3) / 4)) - x)
#define ALIGN(x) x = ((uint8_t *)(4 * ((((uint32_t)x) + 3) / 4)))

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