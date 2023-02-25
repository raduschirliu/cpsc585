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

    ASSERT_MSG(g_vehicle_.initialize(*physicsService_->GetKPhysics(),
                                     PxCookingParams(PxTolerancesScale()),
                                     *physicsService_->GetKMaterial(), true),
               "Vehicle must successfully initialize");

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
    rigidbody->userData = &GetEntity();
    uint32_t num_shapes = rigidbody->getNbShapes();
    PxShape* shape = nullptr;

    for (uint32_t i = 0; i < 1; i++)
    {
        rigidbody->getShapes(&shape, 1, i);

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
    game_state_service_ = &service_provider.GetService<GameStateService>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    ValidFileChecker();

    InitMaterialFrictionTable();
    ASSERT_MSG(InitializeVehicle(), "Vehicle must initialize");
}

void VehicleComponent::OnUpdate(const Timestep& delta_time)
{
    g_vehicle_.step(physicsService_->GetTimeStep(),
                    g_vehicle_simulation_context_);

    const PxTransform& pose =
        g_vehicle_.mPhysXState.physxActor.rigidBody->getGlobalPose();

    // get the speed of the car and store it in our variable which is also being
    // used by PlayerState class.
    *speed_ = g_vehicle_.mPhysXState.physxActor.rigidBody->getLinearVelocity()
                  .magnitude();

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
    physx::PxQuat quat(
        transform_->GetOrientation().x, transform_->GetOrientation().y,
        transform_->GetOrientation().z, transform_->GetOrientation().w);

    PxTransform pose(GlmToPx(transform_->GetPosition()), quat);

    g_vehicle_name_ = vehicle_name;
    g_vehicle_.setUpActor(*physicsService_->GetKScene(), pose,
                          g_vehicle_name_.c_str());

    const physx::PxVec3 pose_transform =
        g_vehicle_.mPhysXState.physxActor.rigidBody->getGlobalPose().p;

    g_vehicle_.mPhysXState.physxActor.rigidBody->setGlobalPose(
        physx::PxTransform(pose_transform, quat));
}
