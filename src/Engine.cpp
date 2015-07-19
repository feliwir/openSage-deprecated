// Stephan Vedder 2015
#include "Engine.hpp"
#include "FileSystem.hpp"
#include "AudioSystem.hpp"
#include "VideoSystem.hpp"
#include "GraphicsSystem.hpp"
#include "Game/Handler.hpp"
#include <SFML/Audio.hpp>

#include <iostream>
#include "Config.hpp"

Engine::Engine()
{
	m_window.create(sf::VideoMode(800, 600), "openSAGE",sf::Style::None);

	AudioSystem::Initialize();
    GraphicsSystem::Initialize();
	VideoSystem::Initialize();
	FileSystem::Initialize();
	Game::Handler::Initialize();
}

Engine::~Engine()
{
	AudioSystem::Release();
	VideoSystem::Release();
    GraphicsSystem::Release();
}

void Engine::Run()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.attributeFlags = sf::ContextSettings::Core;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits = 8;
    settings.stencilBits = 24;

	m_window.create(sf::VideoMode(Config::Width, Config::Height), Config::Title, 
        Config::Fullscreen ? sf::Style::Fullscreen : sf::Style::None,settings);


	while (m_window.isOpen())
	{	
        GraphicsSystem::Clear();
        sf::Event event;

        //check if any events happened and pass them to the handler
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
        GraphicsSystem::Update();
        GraphicsSystem::Render();
		m_window.display();
	}
}
