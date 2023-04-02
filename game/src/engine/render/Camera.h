#pragma once

#include <glm/glm.hpp>
#include <object_ptr.hpp>

#include "engine/core/math/Cuboid.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class RenderService;
class InputService;

enum class CameraType : uint8_t
{
    kDisabled,
    kNormal,
    kDebug,
};

class Camera final : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    Camera();

    void SetFov(float fov_degrees);
    /**
     * Set camera's screen aspect ratio, where `aspect_ratio = width / height`
     */
    void SetAspectRatio(float aspect_ratio);

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDebugGui() override;
    void OnDestroy() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

    void SetType(CameraType type);

    CameraType GetType() const;
    const Cuboid& GetFrustumWorldVertices() const;
    const glm::mat4& GetProjectionMatrix() const;
    const glm::mat4& GetViewMatrix() const;

  private:
    float fov_degrees_;
    float aspect_ratio_;
    float near_plane_;
    float far_plane_;
    Cuboid frustum_world_;
    CameraType type_;

    glm::mat4 projection_matrix_;
    glm::mat4 view_matrix_;
    glm::mat4 inverse_view_proj_;

    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<Transform> transform_;

    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateFrustumVertices();
};
