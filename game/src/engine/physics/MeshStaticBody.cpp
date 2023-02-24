#include "engine/physics/MeshStaticBody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string;
using std::string_view;

using namespace physx;

void MeshStaticBody::OnInit(const ServiceProvider& service_provider)
{
    physics_service_ = &service_provider.GetService<PhysicsService>();

    transform_ = &GetEntity().GetComponent<Transform>();
}

void MeshStaticBody::OnDestroy()
{
    physics_service_->UnregisterActor(static_);
}

string_view MeshStaticBody::GetName() const
{
    return "MeshStaticBody";
}

void MeshStaticBody::SetMesh(const string& name, float scale)
{
    ASSERT_MSG(!static_, "Static rigidbody already exists");
    mesh_name_ = name;

    PxTriangleMesh* mesh = physics_service_->CreateTriangleMesh(name);
    ASSERT(mesh);

    PxTriangleMeshGeometry geometry(mesh, PxMeshScale(scale));
    shape_ = physics_service_->CreateShape(geometry);
    shape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
    shape_->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

    static_ = physics_service_->CreateRigidStatic(transform_->GetPosition(),
                                                  transform_->GetOrientation());
    static_->userData = &GetEntity();
    static_->attachShape(*shape_);

    physics_service_->RegisterActor(static_);
}
