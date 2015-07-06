#include "Engine.hpp"
#include "FileSystem.hpp"
#include "AudioSystem.hpp"
#include "VideoSystem.hpp"
#include "Game/Handler.hpp"
#include <SFML/Audio.hpp>
// (C) Stephan Vedder 2015
#include <iostream>

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
	//m_window.create(sf::VideoMode(1920, 1080), "openSAGE", sf::Style::Fullscreen);

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
