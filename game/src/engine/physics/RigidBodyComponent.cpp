#include "engine/physics/RigidBodyComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using physx::PxTransform;
using std::string_view;

static constexpr float kDefaultDenisty = 10.0f;

void RigidBodyComponent::OnInit(const ServiceProvider& service_provider)
{
    Log::info("RigidBodyComponent - Init");

    physics_service_ = &service_provider.GetService<PhysicsService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    PxTransform pose = CreatePxTransform(transform_->GetPosition(),
                                         transform_->GetOrientation());
    dynamic_ = physics_service_->GetKPhysics()->createRigidDynamic(pose);
    dynamic_->userData = &GetEntity();
    PxRigidBodyExt::updateMassAndInertia(*dynamic_, kDefaultDenisty);

    physics_service_->RegisterActor(dynamic_);
}

void RigidBodyComponent::OnUpdate(const Timestep& delta_time)
{
    const PxTransform pose = dynamic_->getGlobalPose();
    const GlmTransform transform = PxToGlm(pose);
    transform_->SetPosition(transform.position);
    transform_->SetOrientation(transform.orientation);
}

void RigidBodyComponent::SetMass(float mass)
{
    dynamic_->setMass(mass);
}

float RigidBodyComponent::GetMass() const
{
    return dynamic_->getMass();
}