#include "VehicleComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string;
using std::string_view;
using namespace physx;

void VehicleComponent::ValidFileChecker()
{
    // Check that we can read from the json file before continuing.
    BaseVehicleParams base_params;
    if (!readBaseParamsFromJsonFile(g_vehicle_data_path_.c_str(), "Base.json",
                                    base_params))
        ASSERT_MSG(false,
                   "Cannot open Base.json file, error in VehicleComponent.");

    // Check that we can read from the json file before continuing.
    DirectDrivetrainParams direct_drivetrain_params;
    if (!readDirectDrivetrainParamsFromJsonFile(
            g_vehicle_data_path_.c_str(), "DirectDrive.json",
            base_params.axleDescription, direct_drivetrain_params))
    {
        ASSERT_MSG(
            false,
            "Cannot open DirectDrive.json file, error in VehicleComponent.");
    }
}

bool VehicleComponent::InitializeVehicle()
{
    readBaseParamsFromJsonFile(g_vehicle_data_path_.c_str(), "Base.json",
                               g_vehicle_.mBaseParams);
    setPhysXIntegrationParams(
        g_vehicle_.mBaseParams.axleDescription, gPhysXMaterialFrictions_,
        gNbPhysXMaterialFrictions_, gPhysXDefaultMaterialFriction_,
        g_vehicle_.mPhysXParams);
    readDirectDrivetrainParamsFromJsonFile(
        g_vehicle_data_path_.c_str(), "DirectDrive.json",
        g_vehicle_.mBaseParams.axleDescription, g_vehicle_.mDirectDriveParams);

    const bool vehicle_init_status = g_vehicle_.initialize(
        *physicsService_->GetKPhysics(), PxCookingParams(PxTolerancesScale()),
        *physicsService_->GetKMaterial(), true);
    ASSERT_MSG(vehicle_init_status, "Vehicle must successfully initialize");

    g_vehicle_.mTransmissionCommandState.gear =
        PxVehicleDirectDriveTransmissionCommandState::eFORWARD;

    // Set up the simulation context.
    // The snippet is set up with
    // a) z as the longitudinal axis
    // b) x as the lateral axis
    // c) y as the vertical axis.
    // d) metres  as the lengthscale.
    g_vehicle_simulation_context_.setToDefault();
    g_vehicle_simulation_context_.frame.lngAxis = PxVehicleAxes::ePosZ;
    g_vehicle_simulation_context_.frame.latAxis = PxVehicleAxes::eNegX;
    g_vehicle_simulation_context_.frame.vrtAxis = PxVehicleAxes::ePosY;
    g_vehicle_simulation_context_.scale.scale = 1.0f;
    g_vehicle_simulation_context_.gravity = physicsService_->GetGravity();
    g_vehicle_simulation_context_.physxScene = physicsService_->GetKScene();
    g_vehicle_simulation_context_.physxActorUpdateMode =
        PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;

    PxRigidBody* rigidbody = g_vehicle_.mPhysXState.physxActor.rigidBody;
    ASSERT_MSG(rigidbody, "Vehicle must have valid PhysX Actor RigidBody");

    rigidbody->userData = &GetEntity();
    const uint32_t num_shapes = rigidbody->getNbShapes();
    PxShape* shape = nullptr;

    // First shape is the vehicle body, the next 4 should be the wheels.
    // TODO: enabling collision for all shapes makes the vehicle get stuck in
    // the floor?
    for (uint32_t i = 0; i < 1; i++)
    {
        rigidbody->getShapes(&shape, 1, i);
        ASSERT_MSG(shape, "RigidBody Shape must be valid");

        shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
    }

    return true;
}

void VehicleComponent::InitMaterialFrictionTable()
{
    // Each physx material can be mapped to a tire friction value on a per tire
    // basis. If a material is encountered that is not mapped to a friction
    // value, the friction value used is the specified default value. In this
    // snippet there is only a single material so there can only be a single
    // mapping between material and friction. In this snippet the same mapping
    // is used by all tires.
    gPhysXMaterialFrictions_[0].friction = 1.0f;
    gPhysXMaterialFrictions_[0].material = physicsService_->GetKMaterial();
    gPhysXDefaultMaterialFriction_ = 1.0f;
    gNbPhysXMaterialFrictions_ = 1;
}

void VehicleComponent::OnInit(const ServiceProvider& service_provider)
{
    physicsService_ = &service_provider.GetService<PhysicsService>();
    input_service_ = &service_provider.GetService<InputService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    ValidFileChecker();

    InitMaterialFrictionTable();
    const bool init_status = InitializeVehicle();
    ASSERT_MSG(init_status, "Vehicle must initialize");
}

void VehicleComponent::OnUpdate(const Timestep& delta_time)
{
    g_vehicle_.step(physicsService_->GetTimeStep(),
                    g_vehicle_simulation_context_);

    const PxTransform& pose =
        g_vehicle_.mPhysXState.physxActor.rigidBody->getGlobalPose();
    const GlmTransform transform = PxToGlm(pose);
    transform_->SetPosition(transform.position);
    transform_->SetOrientation(transform.orientation);
}

std::string_view VehicleComponent::GetName() const
{
    return "Vehicle";
}

void VehicleComponent::SetVehicleName(const string& vehicle_name)
{
    PxTransform pose(GlmToPx(transform_->GetPosition()), PxQuat(PxIdentity));

    g_vehicle_name_ = vehicle_name;
    g_vehicle_.setUpActor(*physicsService_->GetKScene(), pose,
                          g_vehicle_name_.c_str());
}
