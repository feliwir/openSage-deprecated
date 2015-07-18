// Stephan Vedder 2015
#pragma once
#include "flextGL.h"
#include <array>
#include <map>

namespace Graphics
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        enum Type
        {
            VERTEX_SHADER = 0,
            GEOMETRY_SHADER = 1,
            FRAGMENT_SHADER = 2
        };

        void LoadFromMemory(const Type type, const std::string& content);
        void Link();

        void Use() const;
        void UnUse() const;

        void AddUniform(const std::string& uniform);
        void AddAttribute(const std::string& attribute);

        GLuint Uniform(const std::string& uniform);
        GLuint Attribute(const std::string& attribute);
    private:     
        GLuint m_program;
        std::array<GLuint, 3> m_shaders;
        std::map<std::string, GLint> m_attributeList;
        std::map<std::string, GLint> m_uniformLocationList;
    };
}
