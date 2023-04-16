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
using glm::uvec2;
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
    std::vector<const Entity*> entities;
    std::vector<BufferMeshLayout> layout;
    VertexArray vertex_array;
    VertexBuffer vertex_buffer;
    ElementArrayBuffer element_buffer;
    size_t buffer_size;
    std::string mesh_names;
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
    : fbo_(),
      rbo_(),
      screen_texture_(),
      render_data_(render_data),
      shadow_maps_(shadow_maps),
      meshes_{},
      shader_("resources/shaders/lit.vert", "resources/shaders/lit.frag"),
      debug_shader_("resources/shaders/debug.vert",
                    "resources/shaders/debug.frag"),
      skybox_shader_("resources/shaders/skybox.vert",
                     "resources/shaders/skybox.frag"),
      laser_material_(render_data),
      skybox_buffers_(),
      skybox_texture_(nullptr),
      wireframe_(false),
      min_shadow_bias_(0.005f),
      max_shadow_bias_(0.05f),
      debug_num_draw_calls_(0),
      debug_total_buffer_size_(0),
      last_screen_size_(0, 0)
{
}

GeometryPass::~GeometryPass() = default;

void GeometryPass::RegisterRenderable(const Entity& entity,
                                      const MeshRenderer& renderer)
{
    string mesh_names = "";

    for (const auto& mesh : renderer.GetMeshes())
    {
        mesh_names += fmt::format("@@{}", mesh.mesh->name);
    }

    // Check if this configuration of buffers/meshes exist
    for (auto& mesh : meshes_)
    {
        if (mesh_names == mesh->mesh_names)
        {
            mesh->entities.push_back(&entity);
            return;
        }
    }

    // Create a new one otherwise
    CreateBuffers(entity, renderer);
}

void GeometryPass::CreateBuffers(const Entity& entity,
                                 const MeshRenderer& renderer)
{
    auto data = make_unique<MeshRenderData>(MeshRenderData{
        .entities = {&entity},
        .layout = {},
        .vertex_array = VertexArray(),
        .vertex_buffer = VertexBuffer(),
        .element_buffer = ElementArrayBuffer(),
        .buffer_size = 0,
        .mesh_names = "",
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
        data->mesh_names += fmt::format("@@{}", mesh.mesh->name);
    }

    data->vertex_buffer.Upload(vertices, GL_STATIC_DRAW);
    data->element_buffer.Upload(indices, GL_STATIC_DRAW);
    data->buffer_size =
        vertices.size() * sizeof(Vertex) + indices.size() * sizeof(uint32_t);

    debug_total_buffer_size_ += data->buffer_size;

    VertexArray::Unbind();

    // Add to render list
    meshes_.push_back(std::move(data));
}

void GeometryPass::UnregisterRenderable(const Entity& entity)
{
    const uint32_t target_id = entity.GetId();
    auto meshes_iter = meshes_.begin();

    while (meshes_iter != meshes_.end())
    {
        MeshRenderData& mesh = *meshes_iter->get();
        auto entity_iter = mesh.entities.begin();

        while (entity_iter != mesh.entities.end())
        {
            if ((*entity_iter)->GetId() == target_id)
            {
                mesh.entities.erase(entity_iter);

                if (mesh.entities.size() == 0)
                {
                    meshes_.erase(meshes_iter);
                }

                return;
            }

            entity_iter++;
        }

        meshes_iter++;
    }
}

void GeometryPass::Init()
{
    InitFbo();
    InitSkybox();

    last_screen_size_ = render_data_.screen_size;

    // Setup materials
    laser_material_.LoadAssets(*render_data_.asset_service);
}

void GeometryPass::InitSkybox()
{
    skybox_buffers_.vertex_array.Bind();
    skybox_buffers_.vertex_buffer.Bind();
    skybox_buffers_.element_buffer.Bind();

    skybox_buffers_.vertex_buffer.ConfigureAttribute(0, 3, GL_FLOAT,
                                                     sizeof(float) * 3, 0);
    skybox_buffers_.vertex_buffer.Upload(kSkyboxVertices, GL_STATIC_DRAW);
    skybox_texture_ = &render_data_.asset_service->GetCubemap("skybox");

    VertexArray::Unbind();
}

void GeometryPass::InitFbo()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    // Create render target
    glBindTexture(GL_TEXTURE_2D, screen_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, render_data_.screen_size.x,
                 render_data_.screen_size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Color attachment (texture render target)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           screen_texture_, 0);

    // Create renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          render_data_.screen_size.x,
                          render_data_.screen_size.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach as depth + stencil buffers
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_);

    // Verify that FBO is complete
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    ASSERT_MSG(status == GL_FRAMEBUFFER_COMPLETE,
               "Framebuffer should be valid");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GeometryPass::Render()
{
    debug_num_draw_calls_ = 0;

    CheckScreenResize();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, render_data_.screen_size.x, render_data_.screen_size.y);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_MULTISAMPLE);

    // Disabling back-face culling until we add more faces to the track
    // glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
            RenderParticles(*main_camera);
        }
    }

    glDisable(GL_MULTISAMPLE);

    // Cleanup
    render_data_.debug_draw_list->Clear();
    laser_material_.Clear();
}

void GeometryPass::RenderDebugGui()
{
    ImGui::Checkbox("Wireframe", &wireframe_);
    ImGui::DragFloat("Min Shadow Bias", &min_shadow_bias_, 0.05f, 0.0f, 1.0f);
    ImGui::DragFloat("Max Shadow Bias", &max_shadow_bias_, 0.05f, 0.0f, 1.0f);

    if (ImGui::Button("Recompile Lit Shader"))
    {
        shader_.Recompile();
        laser_material_.RecompileShader();
    }

    ImGui::Text("Draw calls: %zu", debug_num_draw_calls_);
    ImGui::Text("Mesh buffers: %zu", meshes_.size());
    ImGui::Text("Sum of all buffer sizes: %zu bytes", debug_total_buffer_size_);

    if (ImGui::CollapsingHeader("Buffers"))
    {
        for (size_t i = 0; i < meshes_.size(); i++)
        {
            const MeshRenderData* mesh = meshes_[i].get();
            ImGui::BulletText("%zu entities - %zu bytes", mesh->entities.size(),
                              mesh->buffer_size);
        }
    }
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

void GeometryPass::CheckScreenResize()
{
    if (last_screen_size_ == render_data_.screen_size)
    {
        return;
    }

    last_screen_size_ = render_data_.screen_size;

    // Resize screen texture
    glBindTexture(GL_TEXTURE_2D, screen_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, render_data_.screen_size.x,
                 render_data_.screen_size.y, 0, GL_RGBA, GL_FLOAT, nullptr);

    // Resize renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          render_data_.screen_size.x,
                          render_data_.screen_size.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
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
    shader_.SetUniform("uMaxShadowBias", max_shadow_bias_);
    shader_.SetUniform("uMinShadowBias", min_shadow_bias_);

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

    // Lighting information
    shader_.SetUniform("uCameraPos", camera.pos);
    shader_.SetUniform("uAmbientLight", vec3(0.1f, 0.1f, 0.1f));
    shader_.SetUniform("uLight.pos", vec3(0.0f, 30.0f, 0.0f));
    shader_.SetUniform("uLight.diffuse", vec3(0.5f, 0.5f, 0.5f));

    // Render each object for each entity
    for (const auto& obj : meshes_)
    {
        obj->vertex_array.Bind();

        for (const auto entity : obj->entities)
        {
            const MeshRenderer& renderer = entity->GetComponent<MeshRenderer>();
            const Transform& transform = entity->GetComponent<Transform>();

            const mat4& model_matrix = transform.GetModelMatrix();
            // Since we're passing normals in world space, the view matrix =
            // identity, so we don't need to multiply by it
            const mat4 normal_matrix = transform.GetNormalMatrix();

            // Vert shader vars
            shader_.SetUniform("uModelMatrix", model_matrix);
            shader_.SetUniform("uNormalMatrix", normal_matrix);

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
                debug_num_draw_calls_ += 1;
            }
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

    debug_num_draw_calls_ += 1;
}

void GeometryPass::RenderParticles(const Camera& camera)
{
    laser_material_.Prepare();
    laser_material_.Draw(camera);
}

void GeometryPass::ResetState()
{
    meshes_.clear();
}

void GeometryPass::SetWireframe(bool state)
{
    wireframe_ = state;
}

LaserMaterial& GeometryPass::GetLaserMaterial()
{
    return laser_material_;
}

TextureHandle& GeometryPass::GetScreenTexture()
{
    return screen_texture_;
}
