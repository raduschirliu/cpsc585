#include "game/components/BasicComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"

using std::string_view;
PhysicsService physicsService;

using namespace physx;

void BasicComponent::OnInit(const ServiceProvider& service_provider)
{
    Log::info("BasicComponent - Init");

    physicsService = service_provider.GetService<PhysicsService>();

    // creating a plane.
    physicsService.CreatePlaneRigidBody(PxPlane(0.f, 1.f, 0.f, 0.f));

    // creating a sphere and getting its dynamic which will be later used to change the location of sphere.
    PxRigidDynamic* Sphere = physicsService.CreateSphereRigidBody(10.f, PxTransform(0.f, 100.f, 0.f), 10.f, PxVec3(0.f));
    physicsService.UpdateSphereLocation(Sphere, PxTransform(0.f, 200.f, 0.f));
}

void BasicComponent::OnUpdate()
{
    Log::debug("hello");
}

string_view BasicComponent::GetName() const
{
    return "BasicComponent";
}
