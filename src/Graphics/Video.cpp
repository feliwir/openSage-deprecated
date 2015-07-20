// Stephan Vedder 2015
#include "Video.hpp"

using namespace Graphics;

Video::Video()
{
}

bool Video::create(const std::string& name, glm::vec2 pos, glm::vec2 size)
{
    if (!m_videoStream.open(name))
        return false;

    m_colorTex.create(m_videoStream.getSize(), gli::FORMAT_RGB8_UNORM);
    return true;
}

void Video::update()
{
    if (m_videoStream.isUpdated())
    {
        m_colorTex.update(m_videoStream.getColorData());
    }
}

void Video::render(Shader& shader)
{
}