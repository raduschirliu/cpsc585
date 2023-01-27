#include "engine/render/RenderService.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "engine/core/debug/Log.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"

using glm::mat4;
using glm::vec3;
using std::make_unique;

RenderService::RenderService()
    : render_list_{},
      shader_("resources/shaders/default.vert", "resources/shaders/basic.frag")
{
}

void RenderService::RegisterRenderable(const Entity& entity)
{
    // TODO(radu): This shouldn't need to be done here...
    auto data = make_unique<RenderData>(RenderData{
        .entity = &entity,
        .vertex_array = VertexArray(),
        .vertex_buffer = VertexBuffer(),
        .element_buffer = ElementArrayBuffer(),
    });

    // Configure vertex array/buffer and upload data
    data->vertex_array.Bind();
    data->vertex_buffer.Bind();
    data->element_buffer.Bind();

    data->vertex_buffer.ConfigureAttribute(0, 3, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, position));
    data->vertex_buffer.ConfigureAttribute(1, 3, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, normal));
    data->vertex_buffer.ConfigureAttribute(2, 3, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, color));
    data->vertex_buffer.ConfigureAttribute(3, 2, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, uv));

    const Mesh& mesh = entity.GetComponent<MeshRenderer>().GetMesh();
    data->vertex_buffer.Upload(mesh.vertices, GL_STATIC_DRAW);
    data->element_buffer.Upload(mesh.indices, GL_STATIC_DRAW);

    // TODO(radu): Unbind vertex array?

    // Add to render list
    render_list_.push_back(std::move(data));
}

void RenderService::UnregisterRenderable(const Entity& entity)
{
    auto iter = render_list_.begin();

    while (iter < render_list_.end())
    {
        if (iter->get()->entity->GetId())
        {
            render_list_.erase(iter);
            break;
        }

        iter++;
    }
}

void RenderService::RegisterCamera(const Camera& camera)
{
    cameras_.push_back(&camera);
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

    for (auto& camera : cameras_)
    {
        RenderCameraView(*camera);
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

void RenderService::RenderCameraView(const Camera& camera)
{
    const mat4& view_matrix = camera.GetViewMatrix();
    const mat4& projection_matrix = camera.GetProjectionMatrix();

    // Render each object
    for (const auto& obj : render_list_)
    {
        const MeshRenderer& renderer = obj->entity->GetComponent<MeshRenderer>();
        const Transform& transform = obj->entity->GetComponent<Transform>();

        mat4 mvp_matrix =
            projection_matrix * view_matrix * transform.GetModelMatrix();

        shader_.Use();
        shader_.SetUniform("uModelMatrix", mvp_matrix);
        shader_.SetUniform("uViewMatrix", mat4(1.0f));
        shader_.SetUniform("uProjectionMatrix", mat4(1.0f));

        obj->vertex_array.Bind();

        GLsizei index_count =
            static_cast<GLsizei>(renderer.GetMesh().indices.size());

        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }
}