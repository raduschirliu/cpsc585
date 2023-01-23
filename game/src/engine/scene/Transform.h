#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "engine/scene/Component.h"

class Transform final : public Component
{
  public:
    Transform();

    void Translate(const glm::vec3& delta);
    void SetPosition(const glm::vec3& position);
    void Rotate(const glm::quat& delta);
    void RotateEulerDegrees(const glm::vec3& delta_euler_degrees);
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetForwardDirection() const;
    const glm::vec3& GetUpDirection() const;
    const glm::vec3& GetRightDirection() const;
    const glm::mat4& GetModelMatrix() const;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

  private:
    glm::vec3 position_;
    glm::quat orientation_;
    glm::vec3 scale_;
    glm::vec3 forward_dir_;
    glm::vec3 up_dir_;
    glm::vec3 right_dir_;

    glm::mat4 translation_matrix_;
    glm::mat4 rotation_matrix_;
    glm::mat4 scale_matrix_;
    glm::mat4 model_matrix_;

    void UpdateMatrices();
};