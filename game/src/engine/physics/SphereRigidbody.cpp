#include "engine/physics/SphereRigidBody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

using namespace physx;

static constexpr float kDefaultRadius = 5.0f;

void SphereRigidBody::OnInit(const ServiceProvider& service_provider)
{
    RigidBodyComponent::OnInit(service_provider);
    SetRadius(kDefaultRadius);
}

string_view SphereRigidBody::GetName() const
{
    return "SphereRigidBody";
}

void SphereRigidBody::SetRadius(float radius)
{
    radius_ = radius;

    if (shape_)
    {
        dynamic_->detachShape(*shape_);
    }

    physx::PxSphereGeometry geometry(radius);
    shape_ = physics_service_->CreateShape(geometry);
    dynamic_->attachShape(*shape_);
}
