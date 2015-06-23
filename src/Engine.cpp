#include "Engine.hpp"
#include "FileSystem.hpp"
#include "AudioSystem.hpp"
#include "VideoSystem.hpp"
#include "Game/Handler.hpp"
#include <SFML/Audio.hpp>
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
		m_window.clear();
		Game::Handler::Update();
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }
		m_window.display();
	}
}
