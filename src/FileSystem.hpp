#pragma once
#include <string>
#include <vector>
#include <filesystem>

class FileSystem
{
public:
	static void Initialize();
	static inline bool Exists(const std::string file)
	{
		std::tr2::sys::path p(file);
		bool res = std::tr2::sys::exists(p);
		return res;
	}
private:
	static std::vector<std::string> getFiles(const std::string &directory, const std::string& mask);
};