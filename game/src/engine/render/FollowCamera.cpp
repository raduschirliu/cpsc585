#include "engine/render/FollowCamera.h"

#include <glm/gtx/transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/render/Camera.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

using namespace glm;
using glm::mat4;
using glm::vec3;

static constexpr vec3 kUpDirection(0.0f, 1.0f, 0.0f);

FollowCamera::FollowCamera()
    : view_matrix_(1.0f),
      camera_offset_(glm::vec3(0.f, 10.f, 30.f))
{
}

void FollowCamera::OnInit(const ServiceProvider& service_provider)
{
    // Event subscriptions
    GetEventBus().Subscribe<OnUpdateEvent>(this);
    transform_ = &GetEntity().GetComponent<Transform>();
}

void FollowCamera::SetFollowingTransform(Entity& entity)
{
    car_entity_ = &entity;
    // get the transform of the entity
    auto temp_transform_ = &entity.GetComponent<Transform>();
    transform_->SetPosition(temp_transform_->GetPosition());
}

std::string_view FollowCamera::GetName() const
{
    return "Follow Camera";
}

void FollowCamera::OnUpdate(const Timestep& delta_time)
{
    // should not be a null pointer
    if (transform_ && car_entity_)
    {
        transform_->SetPosition(
            car_entity_->GetComponent<Transform>().GetPosition() -
            transform_->GetForwardDirection() * 30.f +
            glm::vec3(0.f, 10.f, 0.f));
    }
}
