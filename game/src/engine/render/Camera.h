#pragma once

#include <glm/glm.hpp>
#include <object_ptr.hpp>

#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class RenderService;
class InputService;

// TODO(radu): Need to have a way to update the Camera's FoV and aspect ratio
class Camera : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    Camera();

    const glm::mat4& GetProjectionMatrix() const;
    const glm::mat4& GetViewMatrix() const;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDebugGui() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

  protected:
    glm::mat4 view_matrix_;
    
  private:
    float fov_degrees_;
    float aspect_ratio_;
    float near_plane_;
    float far_plane_;

    glm::mat4 projection_matrix_;

    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<Transform> transform_;

    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
};
