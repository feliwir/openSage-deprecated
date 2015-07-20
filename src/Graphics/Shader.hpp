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


        void loadFromFile(const Type type, const std::string& filename);
        void loadFromMemory(const Type type, const std::string& content);
        void link();

        void use() const;
        void unUse() const;

        void addUniform(const std::string& uniform);
        void addAttribute(const std::string& attribute);

        GLuint uniform(const std::string& uniform);
        GLuint attribute(const std::string& attribute);
    private:     
        GLuint m_program;
        std::array<GLuint, 3> m_shaders;
        std::map<std::string, GLint> m_attributeList;
        std::map<std::string, GLint> m_uniformLocationList;
    };
}
