#include "engine/render/RenderService.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/render/Camera.h"
#include "engine/render/MeshRenderer.h"
#include "engine/render/PointLight.h"
#include "engine/service/ServiceProvider.h"

using glm::mat4;
using glm::vec3;
using std::make_unique;
using std::unique_ptr;

static VertexArray* kDebugVertexArray = nullptr;
static VertexBuffer* kDebugVertexBuffer = nullptr;

RenderService::RenderService()
    : render_list_{},
      cameras_{},
      lights_{},
      materials_{},
      shader_("resources/shaders/default.vert",
              "resources/shaders/blinnphong.frag"),
      debug_shader_("resources/shaders/debug.vert",
                    "resources/shaders/debug.frag"),
      physics_debug_draw_(false),
      wireframe_(false),
      menu_open_(false)
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
    // TODO: temp physics debug code
    kDebugVertexArray = new VertexArray();
    kDebugVertexBuffer = new VertexBuffer();

    GLsizei stride = sizeof(physx::PxVec3) + sizeof(physx::PxU32);
    GLsizei pos_offset = offsetof(physx::PxDebugLine, pos0);
    GLsizei col_offset = offsetof(physx::PxDebugLine, color0);

    kDebugVertexArray->Bind();
    kDebugVertexBuffer->Bind();
    kDebugVertexBuffer->ConfigureAttribute(0, 3, GL_FLOAT, stride,
                                           pos_offset);  // Pos
    kDebugVertexBuffer->ConfigureAttribute(1, 4, GL_UNSIGNED_BYTE, true, stride,
                                           col_offset);  // Color
}

void RenderService::OnStart(ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    physics_service_ = &service_provider.GetService<PhysicsService>();

    GetEventBus().Subscribe<OnGuiEvent>(this);
}

void RenderService::OnSceneLoaded(Scene& scene)
{
    render_list_.clear();
    lights_.clear();
    cameras_.clear();
}

void RenderService::OnUpdate()
{
    // Debug menu
    if (input_service_->IsKeyPressed(GLFW_KEY_F2))
    {
        menu_open_ = !menu_open_;
    }

    // Rendering
    RenderPrepare();

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (wireframe_)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    for (auto& camera : cameras_)
    {
        RenderCameraView(*camera);
    }
}

void RenderService::OnCleanup()
{
    delete kDebugVertexArray;
    delete kDebugVertexBuffer;
}

std::string_view RenderService::GetName() const
{
    return "RenderService";
}

void RenderService::OnGui()
{
    if (!menu_open_)
    {
        return;
    }

    if (!ImGui::Begin("RenderService", &menu_open_))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Cameras: %zu", cameras_.size());
    ImGui::Text("Lights: %zu", lights_.size());
    ImGui::Text("Meshes: %zu", render_list_.size());

    ImGui::Checkbox("Wireframe", &wireframe_);

    if (ImGui::Checkbox("Physics Debug", &physics_debug_draw_))
    {
        physx::PxScene* scene = physics_service_->GetKScene();
        if (physics_debug_draw_)
        {
            bool status = scene->setVisualizationParameter(
                physx::PxVisualizationParameter::eSCALE, 1.0f);
            status |= scene->setVisualizationParameter(
                physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
            status |= scene->setVisualizationParameter(
                physx::PxVisualizationParameter::eACTOR_AXES, 4.0f);
        }
        else
        {
            scene->setVisualizationParameter(
                physx::PxVisualizationParameter::eSCALE, 0.0f);
        }
    }

    ImGui::End();
}

void RenderService::RenderPrepare()
{
}

void RenderService::RenderCameraView(Camera& camera)
{
    auto& camera_transform = camera.GetEntity().GetComponent<Transform>();
    const vec3& camera_pos = camera_transform.GetPosition();

    const mat4& view_matrix = camera.GetViewMatrix();
    const mat4& projection_matrix = camera.GetProjectionMatrix();
    const mat4 view_proj_matrix = projection_matrix * view_matrix;

    // TODO(radu): Allow for more than one light, and actually use light props
    // auto light_entity = lights_[0];
    // PointLight& light = light_entity->GetComponent<PointLight>();

    shader_.Use();
    shader_.SetUniform("uViewProjMatrix", view_proj_matrix);

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

        const MaterialProperties& material_properties =
            renderer.GetMaterialProperties();

        // TODO(radu): Move this logic to Material

        // Vert shader vars
        shader_.SetUniform("uModelMatrix", model_matrix);
        shader_.SetUniform("uNormalMatrix", normal_matrix);

        // Frags shader vars
        // TODO(radu): Don't hardcode
        shader_.SetUniform("uAmbientLight", vec3(0.1f, 0.1f, 0.1f));
        shader_.SetUniform("uCameraPos", camera_pos);
        shader_.SetUniform("uMaterial.specularColor",
                           material_properties.specular);
        shader_.SetUniform("uMaterial.shininess",
                           material_properties.shininess);
        // shader_.SetUniform("uMaterial.albedoTexture", 0);
        shader_.SetUniform("uMaterial.albedoColor",
                           material_properties.albedo_color);
        shader_.SetUniform("uLight.pos", vec3(0.0f, 30.0f, 0.0f));
        shader_.SetUniform("uLight.diffuse", vec3(0.7f, 0.7f, 0.7f));

        obj->vertex_array.Bind();

        GLsizei index_count =
            static_cast<GLsizei>(renderer.GetMesh().indices.size());

        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }

    // TODO: TEMP, draw physics debug info
    if (physics_debug_draw_)
    {
        physx::PxPhysics* physics = physics_service_->GetKPhysics();
        physx::PxScene* scene = physics_service_->GetKScene();

        const auto& render_buffer = scene->getRenderBuffer();
        const physx::PxDebugLine* lines = render_buffer.getLines();
        const size_t num_lines = render_buffer.getNbLines();
        const size_t num_vertices = num_lines * 2; // 2 vertices per line
        const size_t lines_size = sizeof(physx::PxDebugLine) * num_lines;

        kDebugVertexBuffer->Upload(lines, lines_size, GL_STATIC_DRAW);

        debug_shader_.Use();
        debug_shader_.SetUniform("uViewProjMatrix", view_proj_matrix);
        kDebugVertexArray->Bind();
        glDrawArrays(GL_LINES, 0, num_vertices);
    }
}

void RenderService::RegisterMaterial(unique_ptr<Material> material)
{
    materials_.push_back(std::move(material));
}