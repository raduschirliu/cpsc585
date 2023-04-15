#include "engine/scene/Transform.h"

#include <imgui.h>

#include <iostream>

#include "engine/core/gui/PropertyWidgets.h"

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;

static constexpr vec3 kXAxis(1.0f, 0.0f, 0.0f);
static constexpr vec3 kYAxis(0.0f, 1.0f, 0.0f);
static constexpr vec3 kZAxis(0.0f, 0.0f, 1.0f);

static constexpr vec4 kDefaultForwardDir = vec4(+kZAxis, 0.0f);
static constexpr vec4 kDefaultUpDir = vec4(+kYAxis, 0.0f);
static constexpr vec4 kDefaultRightDir = vec4(+kXAxis, 0.0f);

Transform::Transform()
    : position_(0.0f, 0.0f, 0.0f),
      orientation_(vec3(0.0f, 0.0f, 0.0f)),
      scale_(1.0f, 1.0f, 1.0f),
      forward_dir_(kDefaultForwardDir),
      up_dir_(kDefaultUpDir),
      right_dir_(kDefaultRightDir),
      translation_matrix_(1.0f),
      rotation_matrix_(1.0f),
      scale_matrix_(1.0f),
      model_matrix_(1.0f),
      normal_matrix_(1.0f)
{
}

void Transform::SetScale(const vec3& scale)
{
    scale_ = scale;
    UpdateMatrices();
}

void Transform::Translate(const vec3& delta)
{
    position_ += delta;
    UpdateMatrices();
}

void Transform::SetPosition(const vec3& position)
{
    position_ = position;
    UpdateMatrices();
}

void Transform::LerpPosition(const vec3& target, float t)
{
    position_ = glm::mix(position_, target, t);
    UpdateMatrices();
}

void Transform::SetOrientation(const quat& orientation)
{
    orientation_ = orientation;
    UpdateMatrices();
}

void Transform::SlerpOrientation(const quat& target, float t)
{
    orientation_ = glm::slerp(orientation_, target, t);
    UpdateMatrices();
}

void Transform::Rotate(const quat& delta)
{
    orientation_ = glm::normalize(delta) * orientation_;
    UpdateMatrices();
}

void Transform::RotateEulerDegrees(const vec3& delta_euler_degrees)
{
    // TODO: Change to using quat() constructor that takes euler angles?

    // Apply in order: yaw (around y-axis), pitch (around x-axis), roll (around
    // z-axis)
    const quat yaw =
        glm::angleAxis(glm::radians(delta_euler_degrees.y), kYAxis);
    const quat pitch =
        glm::angleAxis(glm::radians(delta_euler_degrees.x), kXAxis);
    const quat roll =
        glm::angleAxis(glm::radians(delta_euler_degrees.z), kZAxis);

    const quat delta = roll * pitch * yaw;
    Rotate(delta);
}

const vec3& Transform::GetPosition() const
{
    return position_;
}

const quat& Transform::GetOrientation() const
{
    return orientation_;
}

const vec3& Transform::GetForwardDirection() const
{
    return forward_dir_;
}

const vec3& Transform::GetUpDirection() const
{
    return up_dir_;
}

const vec3& Transform::GetRightDirection() const
{
    return right_dir_;
}

const mat4& Transform::GetModelMatrix() const
{
    return model_matrix_;
}

const mat4& Transform::GetNormalMatrix() const
{
    return normal_matrix_;
}

void Transform::OnInit(const ServiceProvider& service_provider)
{
}

void Transform::OnDebugGui()
{
    bool dirty = false;

    dirty |= gui::EditProperty("Position", position_);
    ImGui::Spacing();

    const vec3& orientation = glm::degrees(glm::eulerAngles(orientation_));
    gui::ViewProperty("Orientation (Euler Degrees)", orientation);
    ImGui::Spacing();

    ImGui::Text("Scale");
    dirty |= gui::EditProperty("Scale", scale_);

    if (dirty)
    {
        UpdateMatrices();
    }
}

std::string_view Transform::GetName() const
{
    return "Transform";
}

void Transform::UpdateMatrices()
{
    // std::cout<<"HAPPPPPPPPPPPPPPPPPPPPPPPPPENINGGGGGGGGGGGGGGGGG";
    translation_matrix_ = glm::translate(mat4(1.0f), position_);
    rotation_matrix_ = glm::toMat4(orientation_);
    scale_matrix_ = glm::scale(mat4(1.0f), scale_);

    model_matrix_ = translation_matrix_ * rotation_matrix_ * scale_matrix_;
    normal_matrix_ = glm::transpose(glm::inverse(model_matrix_));

    forward_dir_ = glm::normalize(rotation_matrix_ * kDefaultForwardDir);
    up_dir_ = glm::normalize(rotation_matrix_ * kDefaultUpDir);
    right_dir_ = glm::normalize(rotation_matrix_ * kDefaultRightDir);
}