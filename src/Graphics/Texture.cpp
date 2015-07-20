// Stephan Vedder 2015
#include "Texture.hpp"
#include "flextGL.h"
using namespace Graphics;

Texture::Texture() : m_id(0)
{
}

Texture::~Texture()
{
    if (m_id)
        glDeleteTextures(1, &m_id);
}

bool Texture::loadFromStream(sf::InputStream& stream)
{
    if (m_id)
        glDeleteTextures(1, &m_id);

    if (loadTGA(stream))
        return true;
    else if (loadDDS(stream))
        return true;

    return false;
}

bool Texture::loadTGA(sf::InputStream& stream)
{
  
    uint8_t header[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t bpp = 32;
    uint8_t id = 8;
    uint16_t width;
    uint16_t height;

    stream.read(header, 12);
    stream.read(&width, 2);
    stream.read(&height, 2);
    stream.read(&bpp, 1);
    stream.read(&id, 1);

    uint8_t bytesPP = bpp / 8;
    if (bytesPP<1 || bytesPP>4)
        return false;

    gli::format format = static_cast<gli::format>(bytesPP - 1);
    auto dim = gli::texture2D::dim_type(width, height);
    m_tex = gli::texture2D(format,dim);

    stream.read(m_tex.data(), width*height * 4);

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_tex.data());
    return true;
}

bool Texture::loadDDS(sf::InputStream& stream)
{
    gli::gl GL;
    size_t size = stream.getSize();
    const char* buf = new const char[size];
    m_tex = gli::texture2D(gli::load_dds(buf, size));
    delete[] buf;
    
    if (m_tex.empty())
    {

        return false;
    }
      
    gli::gl::format const format = GL.translate(m_tex.format());

    glGenTextures(1, &m_id);  
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(m_tex.levels() - 1));
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA,reinterpret_cast<const GLint*>(format.Swizzle));
    glTexImage2D(GL_TEXTURE_2D, 0, format.Internal, m_tex.dimensions().x, m_tex.dimensions().y, 0, format.External, GL_UNSIGNED_BYTE, NULL);

    //uncompress texture in the gpu
    if (gli::is_compressed(m_tex.format()))
    {
        //fill all mipmap levels
        for (std::size_t Level = 0; Level < m_tex.levels(); ++Level)
        {
            glCompressedTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(Level),
                0, 0,
                static_cast<GLsizei>(m_tex[Level].dimensions().x),
                static_cast<GLsizei>(m_tex[Level].dimensions().y),
                format.External,
                static_cast<GLsizei>(m_tex[Level].size()),
                m_tex[Level].data());
        }
    }
    else
    {
        //fill all mipmap levels
        for (std::size_t Level = 0; Level < m_tex.levels(); ++Level)
        {
            glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(Level),
                0, 0,
                static_cast<GLsizei>(m_tex[Level].dimensions().x),
                static_cast<GLsizei>(m_tex[Level].dimensions().y),
                format.External, format.Type,
                m_tex[Level].data());
        }
    }

    return true;
}

void Texture::create(gli::texture2D::dim_type dim, gli::format gliFormat)
{
    m_tex = gli::texture2D(gliFormat, dim);
    gli::gl GL;
    gli::gl::format const format = GL.translate(m_tex.format());
    
    glGenTextures(1, &m_id);

    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //create image on the gpu with given size & dataformat
    glTexImage2D(GL_TEXTURE_2D, 0, format.Internal, m_tex.dimensions().x, m_tex.dimensions().y, 0, format.External, GL_UNSIGNED_BYTE, nullptr);
}

void Texture::update(const uint8_t* data)
{
    gli::gl GL;
    gli::gl::format const format = GL.translate(m_tex.format());

    glBindTexture(GL_TEXTURE_2D, m_id);
    //change content of the gpu
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex.dimensions().x, m_tex.dimensions().y, format.External, GL_UNSIGNED_BYTE, data);
}