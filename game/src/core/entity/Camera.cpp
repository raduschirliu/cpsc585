#include "core/entity/Camera.h"

#include "core/Utils.h"

using std::shared_ptr;
using utils::kIdentityMatrix;

Camera::Camera()
    : transform_(),
      viewport_size_(100, 100),
      view_matrix_(kIdentityMatrix),
      projection_matrix_(kIdentityMatrix)
{
}

void Camera::RenderGui()
{
    // To be overridden
}

void Camera::Update(float delta_time)
{
    // To be overridden
}

void Camera::Render()
{
    BuildViewMatrix();
    UpdateShaderUniforms();
}

void Camera::SetViewportSize(glm::ivec2 size)
{
    viewport_size_ = size;
    BuildProjectionMatrix();
}

void Camera::UpdateShaderUniforms()
{
    auto& shader_manager = ShaderManager::Instance();

    // TODO(radu): Consider using uniform blocks?
    for (auto& data : shader_manager.GetPrograms())
    {
        data->shader->Use();
        data->shader->SetUniform("uViewMatrix", view_matrix_);
        data->shader->SetUniform("uProjectionMatrix", projection_matrix_);

        switch (data->type)
        {
            case ShaderManager::ShaderType::kPhong:
                data->shader->SetUniform("uCameraPos",
                                         transform_.GetPosition());
                break;
        }
    }
}