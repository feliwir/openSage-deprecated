// Stephan Vedder 2015
#pragma once
#include "Shader.hpp"

namespace Graphics
{
    class IRenderable
    {
    public:
        virtual void render(Shader& shader) = 0;
        virtual void update() = 0;
    };
}