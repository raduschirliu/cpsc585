#include "engine/physics/BoxTrigger.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using physx::PxBoxGeometry;
using physx::PxTransform;
using physx::PxShapeFlag;
using physx::PxActorFlag;
using std::string_view;

static constexpr vec3 kDefaultSize(5.0f, 5.0f, 5.0f);

void BoxTrigger::OnInit(const ServiceProvider& service_provider)
{
    RigidBodyComponent::OnInit(service_provider);

    SetSize(kDefaultSize);
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
    }

    PxBoxGeometry geometry(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
    shape_ = physics_service_->CreateShape(geometry);
    shape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape_->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
    dynamic_->attachShape(*shape_);
    dynamic_->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
}
