#include "engine/physics/BoxRigidBody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using physx::PxBoxGeometry;
using physx::PxTransform;
using std::string_view;

static constexpr vec3 kDefaultSize(1.0f, 1.0f, 1.0f);

void BoxRigidBody::OnInit(const ServiceProvider& service_provider)
{
    RigidBodyComponent::OnInit(service_provider);

    SetSize(kDefaultSize);
}

string_view BoxRigidBody::GetName() const
{
    return "BoxRigidBody";
}

void BoxRigidBody::OnDestroy()
{
    PX_RELEASE(shape_);

    // Parent OnDestroy releases the RigidDynamic, and we must release our
    // shape first
    RigidBodyComponent::OnDestroy();
}

void BoxRigidBody::SetSize(const vec3& size)
{
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
        PX_RELEASE(shape_);
    }

    PxBoxGeometry geometry(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
    shape_ = physics_service_->CreateShape(geometry);
    dynamic_->attachShape(*shape_);
}

physx::PxShape* BoxRigidBody::GetShape()
{
    return shape_;
}
