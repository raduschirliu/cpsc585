#include "engine/render/RenderService.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "engine/core/debug/Log.h"

using std::make_unique;

RenderService::RenderService()
    : render_list_{},
      shader_("resources/shaders/default.vert", "resources/shaders/basic.frag")
{
}

void RenderService::RegisterRenderable(const RenderableComponent& renderable)
{
    auto data = make_unique<RenderData>(RenderData{
        .renderable = &renderable,
        .vertex_array = VertexArray(),
        .vertex_buffer = VertexBuffer(),
    });

    // Configure vertex array/buffer and upload data
    data->vertex_array.Bind();
    data->vertex_buffer.Bind();

    data->vertex_buffer.ConfigureAttribute(0, 3, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, position));
    data->vertex_buffer.ConfigureAttribute(1, 3, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, normal));
    data->vertex_buffer.ConfigureAttribute(2, 3, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, color));
    data->vertex_buffer.ConfigureAttribute(3, 2, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, uv));

    data->vertex_buffer.Upload(renderable.GetMesh().vertices, GL_STATIC_DRAW);

    // TODO(radu): Test and see if this event works :0

    // Add to render list
    render_list_.push_back(std::move(data));
}

void RenderService::OnInit()
{
    Log::info("RenderService - Initializing");
}

void RenderService::OnStart(ServiceProvider& service_provider)
{
}

void RenderService::OnUpdate()
{
    RenderPrepare();

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view_matrix(1.0f);
    const glm::mat4 projection_matrix(1.0f);

    // Render each object
    for (const auto& obj : render_list_)
    {
        glm::mat4 mvp_matrix =
            projection_matrix * view_matrix * obj->renderable->GetModelMatrix();

        shader_.Use();
        shader_.SetUniform("uModelMatrix", mvp_matrix);
        shader_.SetUniform("uViewMatrix", glm::mat4(1.0f));
        shader_.SetUniform("uProjectionMatrix", glm::mat4(1.0f));

        obj->vertex_array.Bind();
        auto& indices = obj->renderable->GetMesh().indices;

        // TODO(radu): Need to use an actual EBO instead of passing indices this way,
        // it seems to be interpreting them as the wrong type like this
        glDrawElements(GL_TRIANGLES, indices.size(), GL_STATIC_DRAW, &indices);
    }
}

void RenderService::OnCleanup()
{
    Log::info("RenderService - Cleaning up");
}

std::string_view RenderService::GetName() const
{
    return "RenderService";
}

void RenderService::RenderPrepare()
{
}
