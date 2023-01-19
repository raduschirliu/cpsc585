#include "engine/render/Renderer.h"

void Renderer::Init()
{
}

void Renderer::RenderFrame()
{
    // Render
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Render objects
}

void Renderer::Cleanup()
{
}
