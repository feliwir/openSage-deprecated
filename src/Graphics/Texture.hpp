// Stephan Vedder 2015
#pragma once
#include <glm/glm.hpp>
#include <gli/gli.hpp>
#include <SFML/System.hpp>
#include <stdint.h>

typedef uint32_t GLuint;

namespace Graphics
{
    class Texture
    {
    public:
		Texture();
		~Texture();

        bool loadFromStream(sf::InputStream& stream);
        bool loadTGA(sf::InputStream& stream);
        bool loadDDS(sf::InputStream& stream);
        void create(gli::texture2D::dim_type size, gli::format format);

        void bind();
        void update(const uint8_t* data);
    private:
        gli::texture2D m_tex;
        GLuint m_id;
    };
}