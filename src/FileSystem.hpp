// Stephan Vedder 2015
#pragma once
#include <string>
#include <vector>
#include <SFML/System.hpp>
#include <memory>

class FileSystem
{
public:
	static void initialize();
    static std::shared_ptr<sf::InputStream> open(const std::string& name);

private:
	static std::vector<std::string> getFiles(const std::string &directory, const std::string& mask);
};