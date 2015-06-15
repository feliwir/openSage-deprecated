#include "FileSystem.hpp"
#include "Loaders/BigStream.hpp"
#include <SFML/System.hpp>

using namespace std;

#ifdef SFML_SYSTEM_WINDOWS
	#include <filesystem>
	using namespace std::tr2::sys;
#else
	#include <dirent.h>
#endif


void FileSystem::Initialize()
{
	auto files = getFiles(".",".big");
	
	for (const auto& file : files)
		BigStream::AddFile(file);

}

std::vector<std::string> FileSystem::getFiles(const string &directory,const string& mask)
{
	std::vector<std::string> files;
#ifdef SFML_SYSTEM_WINDOWS
	//get all files from the specified directory and it's subdirectories
	for (recursive_directory_iterator i("."), end; i != end; ++i)
		if (!is_directory(i->path()) && i->path().extension()==mask)
			files.push_back(i->path().file_string());
#else
	//TODO:
	//CODE FOR LINUX SYSTEMS & MACOS
#endif

	return files;
}