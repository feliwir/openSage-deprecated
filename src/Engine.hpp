#pragma once
#include <SFML/Graphics.hpp>
#include "Loaders/Vp6Stream.hpp"
#include "Loaders/Mp3Stream.hpp"

class Engine
{
public:
	Engine();
	~Engine();

	void Run();

private:
	sf::RenderWindow m_window;
	Loaders::Mp3Stream mp3;
	Loaders::Vp6Stream vp6;
};