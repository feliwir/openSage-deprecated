// Stephan Vedder 2015
#pragma once
#include <string>
#include <vector>

class FileSystem
{
public:
	static void Initialize();
private:
	static std::vector<std::string> getFiles(const std::string &directory, const std::string& mask);
};