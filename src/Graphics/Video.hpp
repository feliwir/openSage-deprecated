// Stephan Vedder 2015
#pragma once
#include "Renderable.hpp"
#include "Texture.hpp"
#include "../Loaders/Vp6Stream.hpp"
#include <glm/glm.hpp>

namespace Graphics
{
    class Video : public IRenderable
    {
    public:
        enum Status
        {
            Stopped = 0,
            Playing = 1
        };


        Video();

        bool Create(const std::string& name, glm::vec2 pos, glm::vec2 size);
        void Update();
        void Render(Shader& shader);

        inline Status getStatus()
        {
            return static_cast<Status>(m_videoStream.getStatus());
        }

        inline void stop()
        {
            m_videoStream.stop();
        }

        inline void play()
        {
            m_videoStream.play();
        }
    private:
        Texture m_colorTex;
        Texture m_alphaTex;
        Loaders::Vp6Stream m_videoStream;

    };
}
