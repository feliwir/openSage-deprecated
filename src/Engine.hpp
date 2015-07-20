// Stephan Vedder 2015
#pragma once
#include <SFML/Window.hpp>
#include "Loaders/Vp6Stream.hpp"
#include "Loaders/Mp3Stream.hpp"

class Engine
{
public:
	Engine();
	~Engine();

	void run();

private:
	sf::Window m_window;
};