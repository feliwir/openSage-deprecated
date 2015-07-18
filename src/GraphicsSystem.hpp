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
    static void Clear();
    static void Initialize();
    static void Release();

    static void Update();
    static void Render();

    static void AddVideo(std::shared_ptr<Graphics::IRenderable>);
private:
    static std::vector<std::shared_ptr<Graphics::IRenderable>> m_videos;
    static std::vector<std::shared_ptr<Graphics::Shader>> m_shaders;
    static glm::mat4 m_ortho;
};
