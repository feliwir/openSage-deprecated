// Stephan Vedder 2015
#pragma once
#include <SFML/Graphics.hpp>
#include "../Loaders/AptFile.hpp"
namespace Graphics 
{
    class Shape : public sf::Drawable
    {
    public:
        void Create(Loaders::AptFile::GeometryEntry& geometry);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates& states);
        std::vector<sf::ConvexShape> m_primitives;
    };
}