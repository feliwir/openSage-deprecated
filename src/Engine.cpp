#include "Engine.hpp"
#include "FileSystem.hpp"
#include "Audio.hpp"
#include <SFML/Audio.hpp>

Engine::Engine()
{
	Audio::Initialize();
	FileSystem::Initialize();
	stream.open("data\\audio\\speech\\mapunfolding.mp3");
	mp3.openFromStream(stream);
	mp3.play();
	m_window.create(sf::VideoMode(800, 600), "SFML window");
}

Engine::~Engine()
{
}

void Engine::Run()
{
	while (m_window.isOpen())
	{
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }
	}
}
