// Stephan Vedder 2015
#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Graphics/Renderable.hpp"
#include "Graphics/Shader.hpp"

class GraphicsSystem
{
private:
    enum ShaderSystem
    {
        VIDEO_SHADER = 0,
    };

public:
    static void clear();
    static void initialize();
    static void release();

    static void update();
    static void render();

    static void addVideo(std::shared_ptr<Graphics::IRenderable>);

private:
    static std::vector<std::shared_ptr<Graphics::IRenderable>> m_videos;
    static std::vector<std::shared_ptr<Graphics::Shader>> m_shaders;
    static glm::mat4 m_ortho;
};
