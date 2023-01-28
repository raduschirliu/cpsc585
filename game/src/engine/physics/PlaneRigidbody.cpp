#include "engine/physics/PlaneRigidbody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

using namespace physx;

void PlaneRigidbody::OnInit(const ServiceProvider& service_provider)
{
    Log::info("PlaneRigidBody - Init");

    // creating a plane.

    // get the transform
    physicsService_ = &service_provider.GetService<PhysicsService>();

    physicsService_->CreatePlaneRigidBody(PxPlane(0, 1, 0, 0));
}


string_view PlaneRigidbody::GetName() const
{
    return "PlaneRigidbody";
}
