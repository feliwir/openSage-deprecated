// Stephan Vedder 2015
#include "Engine.hpp"
#include "FileSystem.hpp"
#include "AudioSystem.hpp"
#include "VideoSystem.hpp"
#include "Game/Handler.hpp"
#include <SFML/Audio.hpp>

#include <iostream>
#include "Config.hpp"

Engine::Engine()
{
	m_window.create(sf::VideoMode(800, 600), "openSAGE",sf::Style::None);
	sf::Image splash;
	const std::string splashFile = "GermanSplash.jpg";
	bool loaded = splash.loadFromFile(splashFile);
	sf::Texture splashTex;
	splashTex.loadFromImage(splash);
	m_window.draw(sf::Sprite(splashTex));
	m_window.display();
	AudioSystem::Initialize();
	VideoSystem::Initialize();
	FileSystem::Initialize();
	Game::Handler::Initialize();
}

Engine::~Engine()
{
	AudioSystem::Release();
	VideoSystem::Release();
}

void Engine::Run()
{
	m_window.create(sf::VideoMode(Config::Width, Config::Height), Config::Title, Config::Fullscreen ? sf::Style::Fullscreen : sf::Style::None);

	while (m_window.isOpen())
	{	
        sf::Event event;
        while (m_window.pollEvent(event))
        {
			switch (event.type)
			{
			case sf::Event::Closed:
				m_window.close();
				break;
			case sf::Event::KeyPressed:
				Game::Handler::KeyDown(event.key);
				break;
			case sf::Event::KeyReleased:
				Game::Handler::KeyUp(event.key);
				break;
			}
      
        }

		Game::Handler::Update(m_window);
		m_window.display();
	}
}
