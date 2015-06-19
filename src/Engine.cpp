#include "Engine.hpp"
#include "FileSystem.hpp"
#include "Audio.hpp"
#include "Video.hpp"
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
	Audio::Initialize();
	Video::Initialize();
	FileSystem::Initialize();
	mp3.open("data\\audio\\speech\\mapunfolding.mp3");
	vp6.open("data/movies/EALogo.vp6");
	m_window.setFramerateLimit(60);
}

Engine::~Engine()
{
	Audio::Release();
}

void Engine::Run()
{
	mp3.play();
	vp6.play();
	//m_window.create(sf::VideoMode(1920, 1080), "openSAGE", sf::Style::Fullscreen);

	while (m_window.isOpen())
	{
		m_window.clear();

        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }
		sf::Sprite spr(vp6.GetTexture());
		spr.setScale(800.0f / spr.getTextureRect().width, 600.0f / spr.getTextureRect().height);
		m_window.draw(spr);
		m_window.display();
	}
}
