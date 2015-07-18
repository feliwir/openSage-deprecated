// Stephan Vedder 2015
#pragma once
#include "Shader.hpp"

namespace Graphics
{
    class IRenderable
    {
    public:
        virtual void Render(Shader& shader) = 0;
        virtual void Update() = 0;
    };
}