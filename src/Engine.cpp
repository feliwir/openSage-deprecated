#include "Engine.hpp"
#include "FileSystem.hpp"
#include "Audio.hpp"
#include <SFML/Audio.hpp>

Engine::Engine()
{
	m_window.create(sf::VideoMode(800, 600), "openSAGE",sf::Style::None);
	sf::Image splash;
	bool loaded = splash.loadFromFile("GermanSplash.jpg");
	sf::Texture splashTex;
	splashTex.loadFromImage(splash);
	m_window.draw(sf::Sprite(splashTex));
	m_window.display();
	Audio::Initialize();
	FileSystem::Initialize();
	stream.open("data\\audio\\speech\\mapunfolding.mp3");
	mp3.openFromStream(stream);
	mp3.play();
}

Engine::~Engine()
{
	Audio::Release();
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

		m_window.display();
	}
}
