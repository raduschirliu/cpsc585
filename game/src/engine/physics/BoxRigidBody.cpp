#include "engine/physics/BoxRigidBody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using physx::PxTransform;
using std::string_view;

static constexpr vec3 kDefaultSize(5.0f, 5.0f, 5.0f);

void BoxRigidBody::OnInit(const ServiceProvider& service_provider)
{
    RigidBodyComponent::OnInit(service_provider);

    SetSize(kDefaultSize);
}

string_view BoxRigidBody::GetName() const
{
    return "BoxRigidBody";
}

void BoxRigidBody::SetSize(const vec3& size)
{
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
    }

    shape_ = physics_service_->CreateShapeCube(size.x, size.y, size.z);
    dynamic_->attachShape(*shape_);
}
