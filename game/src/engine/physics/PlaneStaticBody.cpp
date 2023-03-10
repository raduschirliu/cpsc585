#include "engine/physics/PlaneStaticBody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

using namespace physx;

void PlaneStaticBody::OnInit(const ServiceProvider& service_provider)
{
    physics_service_ = &service_provider.GetService<PhysicsService>();

    // Infinite plane in XY
    PxPlane plane_dimensions(0.0f, 1.0f, 0.0f, 0.0f);
    static_ = physics_service_->CreatePlaneRigidStatic(plane_dimensions);
    static_->userData = &GetEntity();

    physics_service_->RegisterActor(static_);
}

void PlaneStaticBody::OnDestroy()
{
    physics_service_->UnregisterActor(static_);

    PX_RELEASE(static_);
}

string_view PlaneStaticBody::GetName() const
{
    return "PlaneStaticBody";
}
