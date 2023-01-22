#pragma once

#include <glm/glm.hpp>
#include <object_ptr.hpp>

#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class RenderService;
class InputService;

// TODO(radu): Need to have a way to update the Camera's FoV and aspect ratio
class Camera final : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    Camera();

    const glm::mat4& GetProjectionMatrix() const;
    const glm::mat4& GetViewMatrix() const;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate() override;

  private:
    float fov_degrees_;
    float aspect_ratio_;
    float near_plane_;
    float far_plane_;

    glm::mat4 projection_matrix_;
    glm::mat4 view_matrix_;

    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<Transform> transform_;

    void UpdateViewMatrix();
};
