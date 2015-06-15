#pragma once
#include <fstream>

namespace Util
{
	template<class T>
	inline T Read(std::ifstream& fin)
	{
		T result;
		fin.read((char*)&result, sizeof(T));
		return result;
	}

	inline std::string ReadString(std::ifstream& fin)
	{
		std::string result;
		char c;
		while (c=fin.get())
			result.push_back(c);

		return result;
	}

}