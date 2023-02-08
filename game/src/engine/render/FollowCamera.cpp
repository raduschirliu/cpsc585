#include "FollowCamera.h"
#include "engine/core/debug/Log.h"


using namespace glm;

static constexpr vec3 kUpDirection(0.0f, 1.0f, 0.0f);

void FollowCamera::UpdateViewMatrix()
{
    const vec3& position = transform_->GetPosition() + vec3(0.f, 20.f, 10.f);
    const vec3& forward = transform_->GetForwardDirection();
    view_matrix_ = glm::lookAt(position, position + forward, kUpDirection);

    //std::cout << "camera : " << forward << std::endl;
}