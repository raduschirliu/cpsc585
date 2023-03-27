#include "engine/render/RenderService.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "engine/asset/AssetService.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/PointLight.h"
#include "engine/service/ServiceProvider.h"

using glm::mat4;
using glm::vec3;
using std::make_unique;
using std::unique_ptr;
using std::vector;

const static vector<float> kSkyboxVertices = {
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,
    -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
    -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

RenderService::RenderService()
    : render_list_{},
      cameras_{},
      lights_{},
      materials_{},
      shader_("resources/shaders/default.vert",
              "resources/shaders/blinnphong.frag"),
      debug_shader_("resources/shaders/debug.vert",
                    "resources/shaders/debug.frag"),
      skybox_shader_("resources/shaders/skybox.vert",
                     "resources/shaders/skybox.frag"),
      skybox_buffers_(),
      skybox_texture_(nullptr),
      debug_draw_list_(),
      wireframe_(false),
      show_debug_menu_(false)
{
}

void RenderService::RegisterRenderable(const Entity& entity,
                                       const MeshRenderer& renderer)
{
    auto data = make_unique<RenderData>(RenderData{
        .entity = &entity,
        .layout = {},
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
    data->vertex_buffer.ConfigureAttribute(2, 2, GL_FLOAT, sizeof(Vertex),
                                           offsetof(Vertex, uv));

    constexpr size_t index_size = sizeof(uint32_t);
    vector<Vertex> vertices;
    vector<uint32_t> indices;

    for (const auto& mesh : renderer.GetMeshes())
    {
        const uint32_t vertex_offset = static_cast<uint32_t>(vertices.size());
        const uint32_t index_offset = static_cast<uint32_t>(indices.size());

        BufferMeshLayout layout = {
            .index_offset = indices.size() * index_size,
            .index_count = mesh.mesh->indices.size(),
        };

        vertices.insert(vertices.end(), mesh.mesh->vertices.begin(),
                        mesh.mesh->vertices.end());
        indices.insert(indices.end(), mesh.mesh->indices.begin(),
                       mesh.mesh->indices.end());

        for (size_t i = 0; i < layout.index_count; i++)
        {
            indices[index_offset + i] += vertex_offset;
        }

        data->layout.push_back(layout);
    }

    data->vertex_buffer.Upload(vertices, GL_STATIC_DRAW);
    data->element_buffer.Upload(indices, GL_STATIC_DRAW);

    VertexArray::Unbind();

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

void RenderService::RegisterCamera(Camera& camera)
{
    cameras_.push_back(&camera);
}

void RenderService::UnregisterCamera(Camera& camera)
{
    const uint32_t camera_entity_id = camera.GetEntity().GetId();
    auto iter = cameras_.begin();

    while (iter != cameras_.end())
    {
        if (iter->get()->GetEntity().GetId() == camera_entity_id)
        {
            cameras_.erase(iter);
            break;
        }

        iter++;
    }
}

void RenderService::RegisterLight(Entity& entity)
{
    lights_.push_back(&entity);
}

void RenderService::UnregisterLight(Entity& entity)
{
    auto iter = lights_.begin();

    while (iter != lights_.end())
    {
        if (iter->get()->GetId() == entity.GetId())
        {
            lights_.erase(iter);
            break;
        }

        iter++;
    }
}

void RenderService::OnInit()
{
}

void RenderService::OnStart(ServiceProvider& service_provider)
{
    // Service dependencies
    input_service_ = &service_provider.GetService<InputService>();
    asset_service_ = &service_provider.GetService<AssetService>();

    // Events
    GetEventBus().Subscribe<OnGuiEvent>(this);

    // Setup skybox
    skybox_buffers_.vertex_array.Bind();
    skybox_buffers_.vertex_buffer.Bind();
    skybox_buffers_.element_buffer.Bind();

    skybox_buffers_.vertex_buffer.ConfigureAttribute(0, 3, GL_FLOAT,
                                                     sizeof(float) * 3, 0);
    skybox_buffers_.vertex_buffer.Upload(kSkyboxVertices, GL_STATIC_DRAW);
    skybox_texture_ = &asset_service_->GetCubemap("skybox");

    VertexArray::Unbind();
}

void RenderService::OnSceneLoaded(Scene& scene)
{
    render_list_.clear();
    lights_.clear();
    cameras_.clear();
}

void RenderService::OnUpdate()
{
    num_draw_calls_ = 0;

    // Debug menu
    if (input_service_->IsKeyPressed(GLFW_KEY_F2))
    {
        show_debug_menu_ = !show_debug_menu_;
    }

    // Rendering
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disabling back-face culling until we add more faces to the track
    // glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    RenderPrepare();

    for (auto& camera : cameras_)
    {
        PrepareCameraView(*camera);
        RenderCameraView(*camera);
    }

    // Post-render cleanup
    debug_draw_list_.Clear();

    glDisable(GL_MULTISAMPLE);
}

void RenderService::OnCleanup()
{
}

std::string_view RenderService::GetName() const
{
    return "RenderService";
}

void RenderService::OnGui()
{
    if (!show_debug_menu_)
    {
        return;
    }

    if (!ImGui::Begin("RenderService Debug", &show_debug_menu_))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Cameras: %zu", cameras_.size());
    ImGui::Text("Lights: %zu", lights_.size());
    ImGui::Text("Meshes: %zu", render_list_.size());
    ImGui::Text("Draw calls: %zu", num_draw_calls_);

    ImGui::Checkbox("Wireframe", &wireframe_);

    ImGui::End();
}

DebugDrawList& RenderService::GetDebugDrawList()
{
    return debug_draw_list_;
}

void RenderService::RenderPrepare()
{
    debug_draw_list_.Prepare();
}

void RenderService::PrepareCameraView(Camera& camera)
{
    render_pass_data_.camera_transform =
        &camera.GetEntity().GetComponent<Transform>();
    render_pass_data_.camera_pos =
        render_pass_data_.camera_transform->GetPosition();

    render_pass_data_.view_matrix = camera.GetViewMatrix();
    render_pass_data_.proj_matrix = camera.GetProjectionMatrix();
    render_pass_data_.view_proj_matrix =
        render_pass_data_.proj_matrix * render_pass_data_.view_matrix;
}

void RenderService::RenderCameraView(Camera& camera)
{
    // Geometry pass
    if (wireframe_)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // TODO(radu): Allow for more than one light, and actually use light props
    // auto light_entity = lights_[0];
    // PointLight& light = light_entity->GetComponent<PointLight>();

    shader_.Use();
    shader_.SetUniform("uViewProjMatrix", render_pass_data_.view_proj_matrix);

    // Render each object
    for (const auto& obj : render_list_)
    {
        const MeshRenderer& renderer =
            obj->entity->GetComponent<MeshRenderer>();
        const Transform& transform = obj->entity->GetComponent<Transform>();

        const mat4& model_matrix = transform.GetModelMatrix();
        // Since we're passing normals in world space, the view matrix =
        // identity, so we don't need to multiply by it
        const mat4 normal_matrix = transform.GetNormalMatrix();

        // Vert shader vars
        shader_.SetUniform("uModelMatrix", model_matrix);
        shader_.SetUniform("uNormalMatrix", normal_matrix);

        // Lighting information
        shader_.SetUniform("uCameraPos", render_pass_data_.camera_pos);
        shader_.SetUniform("uAmbientLight", vec3(0.1f, 0.1f, 0.1f));
        shader_.SetUniform("uLight.pos", vec3(0.0f, 30.0f, 0.0f));
        shader_.SetUniform("uLight.diffuse", vec3(0.5f, 0.5f, 0.5f));

        obj->vertex_array.Bind();

        // Draw all meshes that are part of this object
        const auto& meshes = renderer.GetMeshes();
        ASSERT_MSG(meshes.size() == obj->layout.size(),
                   "Mesh data out of sync with renderer");

        for (size_t i = 0; i < meshes.size(); i++)
        {
            const MaterialProperties& material_properties =
                meshes[i].material_properties;

            if (material_properties.albedo_texture)
            {
                material_properties.albedo_texture->Bind();
            }

            shader_.SetUniform("uMaterial.specularColor",
                               material_properties.specular);
            shader_.SetUniform("uMaterial.shininess",
                               material_properties.shininess);
            shader_.SetUniform("uMaterial.albedoTexture", 0);
            shader_.SetUniform("uMaterial.albedoColor",
                               material_properties.albedo_color);

            GLsizei index_count =
                static_cast<GLsizei>(obj->layout[i].index_count);
            void* index_offset = reinterpret_cast<void*>(
                static_cast<intptr_t>(obj->layout[i].index_offset));

            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT,
                           index_offset);

            num_draw_calls_++;
        }
    }

    // Debug items
    RenderDebugDrawList();
    RenderSkybox();
}

void RenderService::RegisterMaterial(unique_ptr<Material> material)
{
    materials_.push_back(std::move(material));
}

void RenderService::RenderDebugDrawList()
{
    if (debug_draw_list_.HasItems())
    {
        debug_shader_.Use();
        debug_shader_.SetUniform("uViewProjMatrix",
                                 render_pass_data_.view_proj_matrix);
        debug_draw_list_.Draw();

        num_draw_calls_++;
    }
}

void RenderService::RenderSkybox()
{
    skybox_buffers_.vertex_array.Bind();

    const mat4 view_no_transform =
        glm::mat4(glm::mat3(render_pass_data_.view_matrix));
    const mat4 view_proj = render_pass_data_.proj_matrix * view_no_transform;

    skybox_shader_.Use();
    skybox_shader_.SetUniform("uViewProjMatrix", view_proj);

    skybox_texture_->Bind();

    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, kSkyboxVertices.size());
    glDepthFunc(GL_LESS);
}
