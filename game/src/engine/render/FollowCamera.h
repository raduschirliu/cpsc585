#pragma once

#include <glm/glm.hpp>
#include <object_ptr.hpp>

#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class RenderService;
class InputService;

class FollowCamera : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    FollowCamera();

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

    // Set the entity from which we want to follow using this camera
    void SetFollowingTransform(Entity& entity);

  protected:
    glm::mat4 view_matrix_;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<Entity> car_entity_;

    glm::vec3 camera_offset_;

    // seperate public functions for getters and setters
  public:
    void SetCameraOffset(glm::vec3 offset)
    {
        camera_offset_ = offset;
    }
};