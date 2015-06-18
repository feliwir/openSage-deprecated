#include "FileSystem.hpp"
#include "Loaders/BigStream.hpp"
#include <SFML/System.hpp>
#include <iostream>

using namespace std;

#ifdef SFML_SYSTEM_WINDOWS
//requires atleast msvc 2013
#include <filesystem>
using namespace std::tr2::sys;
#else
//requires latest gcc/g++! (build from github)
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
#endif


void FileSystem::Initialize()
{
	auto files = getFiles(".",".big");
	
	for (const auto& file : files)
		BigStream::AddFile(file);

	std::cout << "FileSystem: Current Workingdir: " << current_path().string() << std::endl;
	std::cout << "FileSystem: Added " << files.size() << " BigArchives" << std::endl;
}

std::vector<std::string> FileSystem::getFiles(const string &directory,const string& mask)
{
	std::vector<std::string> files;

	//get all files from the specified directory and it's subdirectories
	for (recursive_directory_iterator i("."), end; i != end; ++i)
		if (!is_directory(i->path()) && i->path().extension()==mask)
			files.push_back(i->path().string());

	return files;
}