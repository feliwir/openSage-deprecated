// Stephan Vedder 2015
#include "Video.hpp"

using namespace Graphics;

const std::string Video::VertexShader = 
""
""
""
"";

const std::string Video::FragmentShader =
""
""
""
"";

Video::Video()
{

}

bool Video::Create(const std::string& name, glm::vec2 pos, glm::vec2 size)
{
    if (!m_videoStream.open(name))
        return false;

    m_colorTex.create(m_videoStream.getSize(), gli::FORMAT_RGB8_UNORM);
    return true;
}

void Video::Update()
{
    
    if (m_videoStream.isUpdated())
    {
        m_colorTex.update(m_videoStream.getColorData());
    }
}

void Video::Render(Shader& shader)
{

}