// Stephan Vedder 2015
#pragma once
#include <string>
#include <vector>
#include <SFML/System.hpp>
#include <memory>

class FileSystem
{
public:
	static void Initialize();
    static std::shared_ptr<sf::InputStream> Open(const std::string& name);
private:
	static std::vector<std::string> getFiles(const std::string &directory, const std::string& mask);
};