#include "engine/physics/BoxTrigger.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using physx::PxActorFlag;
using physx::PxBoxGeometry;
using physx::PxShapeFlag;
using physx::PxTransform;
using std::string_view;

static constexpr vec3 kDefaultSize(5.0f, 5.0f, 5.0f);

void BoxTrigger::OnInit(const ServiceProvider& service_provider)
{
    RigidBodyComponent::OnInit(service_provider);

    SetSize(kDefaultSize);
    SetGravityEnabled(false);
}

void BoxTrigger::OnDestroy()
{
    PX_RELEASE(shape_);

    // Parent OnDestroy releases the RigidDynamic, and we must release our
    // shape first
    RigidBodyComponent::OnDestroy();
}

string_view BoxTrigger::GetName() const
{
    return "BoxTrigger";
}

void BoxTrigger::SetSize(const vec3& size)
{
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
        PX_RELEASE(shape_);
    }

    PxBoxGeometry geometry(size.x, size.y, size.z);
    shape_ = physics_service_->CreateShape(geometry);
    shape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape_->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
    // so that shooting queries don't get blocked
    shape_->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

    dynamic_->attachShape(*shape_);
}
