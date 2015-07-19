// Stephan Vedder 2015
#include "Video.hpp"

using namespace Graphics;

const std::string Video::VertexShader =
"#version 330 core\n"
"in vec2 pos;\n"
"uniform vec2 translate;\n"
"uniform mat4 ortho;\n"
"void main(void) {\n"
"vec2 tran_pos = vec2(pos + vec2(translate));\n"
"gl_Position = ortho * vec4(tran_pos,1.0f, 1.0f);\n"
"}\n";

const std::string Video::FragmentShader =
"#version 330 core\n"
"out vec4 color\n;"
"void main(void) {\n"
"color = vec4(0,0,0,1);\n"
""
"}\n";

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