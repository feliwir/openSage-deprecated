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

	AudioSystem::initialize();
    GraphicsSystem::initialize();
	VideoSystem::initialize();
	FileSystem::initialize();
	Game::Handler::initialize();
}

Engine::~Engine()
{
	AudioSystem::release();
	VideoSystem::release();
    GraphicsSystem::release();
}

void Engine::run()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.attributeFlags = sf::ContextSettings::Core;
    settings.majorVersion = 4;
    settings.minorVersion = 0;
    settings.depthBits = 8;
    settings.stencilBits = 24;

	m_window.create(sf::VideoMode(Config::width, Config::height), Config::title, 
        Config::fullscreen ? sf::Style::Fullscreen : sf::Style::None,settings);


	while (m_window.isOpen())
	{	
        GraphicsSystem::clear();
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
				Game::Handler::keyDown(event.key);
				break;
			case sf::Event::KeyReleased:
				Game::Handler::keyUp(event.key);
				break;
			}
      
        }
		Game::Handler::update(m_window);
        GraphicsSystem::update();
        GraphicsSystem::render();
		m_window.display();
	}
}
