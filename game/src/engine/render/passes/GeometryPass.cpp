#include "engine/render/passes/GeometryPass.h"

#include <imgui.h>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/Cubemap.h"
#include "engine/core/gfx/ShaderProgram.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/passes/depth/ShadowMap.h"
#include "engine/scene/Entity.h"

using glm::ivec2;
using glm::mat4;
using glm::vec3;
using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

struct BufferMeshLayout
{
    size_t index_offset;
    size_t index_count;
};

struct MeshRenderData
{
    const Entity* entity;
    std::vector<BufferMeshLayout> layout;
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
};

struct CameraView
{
    Transform* camera_transform;
    glm::vec3 pos;
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
    glm::mat4 view_proj_matrix;
};

constexpr int kShadowMapTextureStart =
    5;  // Arbitrary choice, normal albedo texture is at idx=0
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

GeometryPass::GeometryPass(SceneRenderData& render_data,
                           const vector<unique_ptr<ShadowMap>>& shadow_maps)
    : render_data_(render_data),
      shadow_maps_(shadow_maps),
      meshes_{},
      shader_("resources/shaders/lit.vert", "resources/shaders/lit.frag"),
      debug_shader_("resources/shaders/debug.vert",
                    "resources/shaders/debug.frag"),
      skybox_shader_("resources/shaders/skybox.vert",
                     "resources/shaders/skybox.frag"),
      wireframe_(false),
      skybox_buffers_(),
      skybox_texture_(nullptr)
{
}

GeometryPass::~GeometryPass() = default;

void GeometryPass::RegisterRenderable(const Entity& entity,
                                      const MeshRenderer& renderer)
{
    auto data = make_unique<MeshRenderData>(MeshRenderData{
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
    meshes_.push_back(std::move(data));
}

void GeometryPass::UnregisterRenderable(const Entity& entity)
{
    const uint32_t target_id = entity.GetId();
    std::erase_if(render_data_.entities, [target_id](const Entity* x)
                  { return x->GetId() == target_id; });
}

void GeometryPass::Init()
{
    // Setup skybox
    skybox_buffers_.vertex_array.Bind();
    skybox_buffers_.vertex_buffer.Bind();
    skybox_buffers_.element_buffer.Bind();

    skybox_buffers_.vertex_buffer.ConfigureAttribute(0, 3, GL_FLOAT,
                                                     sizeof(float) * 3, 0);
    skybox_buffers_.vertex_buffer.Upload(kSkyboxVertices, GL_STATIC_DRAW);
    skybox_texture_ = &render_data_.asset_service->GetCubemap("skybox");

    VertexArray::Unbind();
}

void GeometryPass::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, render_data_.screen_size.x, render_data_.screen_size.y);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Disabling back-face culling until we add more faces to the track
    // glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Determine which camera to render
    if (render_data_.cameras.size() > 0)
    {
        Camera* main_camera = nullptr;

        for (size_t i = 0; i < render_data_.cameras.size(); i++)
        {
            Camera* camera = render_data_.cameras[i];

            if (camera->GetType() == CameraType::kDisabled)
            {
                // Ignore disabled cameras
            }
            else if (camera->GetType() == CameraType::kNormal)
            {
                main_camera = camera;
            }
            else if (camera->GetType() == CameraType::kDebug)
            {
                // First debug camera always gets priority
                main_camera = camera;
                break;
            }
        }

        if (main_camera)
        {
            CameraView view = PrepareCameraView(*main_camera);
            RenderMeshes(view);
            RenderDebugDrawList(view);
            RenderSkybox(view);
        }
    }

    glDisable(GL_MULTISAMPLE);

    // Cleanup
    render_data_.debug_draw_list->Clear();
}

void GeometryPass::RenderDebugGui()
{
    ImGui::Checkbox("Wireframe", &wireframe_);
}

CameraView GeometryPass::PrepareCameraView(Camera& camera)
{
    CameraView view = {};

    view.camera_transform = &camera.GetEntity().GetComponent<Transform>();
    view.pos = view.camera_transform->GetPosition();

    view.view_matrix = camera.GetViewMatrix();
    view.proj_matrix = camera.GetProjectionMatrix();
    view.view_proj_matrix = view.proj_matrix * view.view_matrix;

    return view;
}

void GeometryPass::RenderMeshes(const CameraView& camera)
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
    shader_.SetUniform("uViewMatrix", camera.view_matrix);
    shader_.SetUniform("uProjMatrix", camera.proj_matrix);

    for (size_t i = 0; i < shadow_maps_.size(); i++)
    {
        const int texture_index = kShadowMapTextureStart + static_cast<int>(i);
        const ShadowMap& map = *shadow_maps_[i];

        glActiveTexture(GL_TEXTURE0 + texture_index);
        glBindTexture(GL_TEXTURE_2D, map.GetTexture());

        const string shadow_map_uniform = fmt::format("uShadowMaps[{}]", i);
        const string light_view_uniform =
            fmt::format("uLightSpaceMatrices[{}]", i);

        shader_.SetUniform(shadow_map_uniform, texture_index);
        shader_.SetUniform(light_view_uniform, map.GetTransformation());
    }

    // Render each object
    for (const auto& obj : meshes_)
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
        shader_.SetUniform("uCameraPos", camera.pos);
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
                material_properties.albedo_texture->Bind(0);
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
        }
    }
}

void GeometryPass::RenderDebugDrawList(const CameraView& camera)
{
    if (render_data_.debug_draw_list->HasItems())
    {
        render_data_.debug_draw_list->Prepare();
        debug_shader_.Use();
        debug_shader_.SetUniform("uViewProjMatrix", camera.view_proj_matrix);
        render_data_.debug_draw_list->Draw();
    }
}

void GeometryPass::RenderSkybox(const CameraView& camera)
{
    skybox_buffers_.vertex_array.Bind();

    const mat4 view_no_transform = glm::mat4(glm::mat3(camera.view_matrix));
    const mat4 view_proj = camera.proj_matrix * view_no_transform;

    skybox_shader_.Use();
    skybox_shader_.SetUniform("uViewProjMatrix", view_proj);

    skybox_texture_->Bind();

    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, kSkyboxVertices.size());
    glDepthFunc(GL_LESS);
}

void GeometryPass::ResetState()
{
    meshes_.clear();
}

void GeometryPass::SetWireframe(bool state)
{
    wireframe_ = state;
}
