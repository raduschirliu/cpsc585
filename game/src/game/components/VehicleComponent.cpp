#include "VehicleComponent.h"

#include <imgui.h>
#include <physx/CommonVehicleFiles/SnippetVehicleHelpers.h>
#include <physx/CommonVehicleFiles/directdrivetrain/DirectDrivetrain.h>
#include <physx/CommonVehicleFiles/serialization/BaseSerialization.h>
#include <physx/CommonVehicleFiles/serialization/DirectDrivetrainSerialization.h>
#include <physx/CommonVehicleFiles/serialization/IntegrationSerialization.h>

#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/core/math/Physx.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/render/ParticleSystem.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "game/components/audio/AudioEmitter.h"
#include "game/components/state/PlayerState.h"
#include "game/services/GameStateService.h"

using glm::vec3;
using std::string;
using std::string_view;
using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

static constexpr PxReal kDefaultMaterialFriction = 1.0f;
static constexpr float kRespawnSeconds = 3.0f;
static float kExhaustParticleDelayMax = 0.25f;
static float kExhaustParticleDelayMin = 0.10f;
static vec3 kExhaustParticleOffset(2.0f, 2.5f, -4.0f);

// filenames and paths

static constexpr const char* kVehicleDataPath = "resources/vehicle_data";
static constexpr const char* kBaseParamFileName = "Base.jsonc";
static constexpr const char* kDirectDriveParamFileName = "DirectDrive.jsonc";
static constexpr const char* kIntegrationParamFileName = "Integration.jsonc";
static constexpr const char* kDrivingAudio = "kart_driving_01.ogg";
static constexpr const char* kRespawnAudio = "kart_respawn_01.ogg";

/* ----- From Component ----- */

void VehicleComponent::OnInit(const ServiceProvider& service_provider)
{
    // service and component dependencies
    physics_service_ = &service_provider.GetService<PhysicsService>();
    input_service_ = &service_provider.GetService<InputService>();
    game_state_service_ = &service_provider.GetService<GameStateService>();
    render_service_ = &service_provider.GetService<RenderService>();

    transform_ = &GetEntity().GetComponent<Transform>();
    audio_emitter_ = &GetEntity().GetComponent<AudioEmitter>();

    // reset cooldown
    respawn_timer_ = 0.0f;

    // subscribe to events
    GetEventBus().Subscribe<OnUpdateEvent>(this);
    GetEventBus().Subscribe<OnPhysicsUpdateEvent>(this);

    // init vehicle properties
    InitMaterialFrictionTable();
    LoadParams();
    InitVehicle();

    physics_service_->RegisterVehicle(&vehicle_, &GetEntity());
    exhaust_particles_ = &render_service_->GetParticleSystem("exhaust");
    exhaust_delay_ = kExhaustParticleDelayMax;

    // init sounds
    audio_emitter_->AddSource(kDrivingAudio);
    audio_emitter_->SetGain(kDrivingAudio, 0.07f);
    audio_emitter_->SetLoop(kDrivingAudio, true);
    audio_emitter_->PlaySource(kDrivingAudio);

    audio_emitter_->AddSource(kRespawnAudio);

    // speed adjuster for the AI.
    speed_adjuster_ = rand() % 60;
}

std::string_view VehicleComponent::GetName() const
{
    return "Vehicle";
}

void VehicleComponent::OnDebugGui()
{
    gui::EditProperty("Exhaust Particle Offset", kExhaustParticleOffset);
    ImGui::DragFloat("Exhaust Particle Delay Max", &kExhaustParticleDelayMax,
                     0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("Exhaust Particle Delay Max", &kExhaustParticleDelayMin,
                     0.01f, 0.0f, 5.0f);

    ImGui::Text("Gear: %d", vehicle_.mTransmissionCommandState.gear);
    ImGui::Text("Steer: %f", vehicle_.mCommandState.steer);
    ImGui::Text("Throttle: %f", vehicle_.mCommandState.throttle);
    ImGui::Text("Front Brake: %f", vehicle_.mCommandState.brakes[0]);
    ImGui::Text("Rear Brake: %f", vehicle_.mCommandState.brakes[1]);

    const vec3 linear_velocity =
        PxToGlm(vehicle_.mBaseState.rigidBodyState.linearVelocity);
    gui::ViewProperty("Linear Velocity", linear_velocity);

    const float speed = glm::length(linear_velocity);
    ImGui::Text("Speed: %f", speed);

    auto vehicle_frame = vehicle_.mBaseParams.frame;
    const float lat_speed =
        vehicle_.mBaseState.rigidBodyState.getLateralSpeed(vehicle_frame);
    const float long_speed =
        vehicle_.mBaseState.rigidBodyState.getLongitudinalSpeed(vehicle_frame);

    ImGui::Text("Lat Speed: %f", lat_speed);
    ImGui::Text("Long Speed: %f", long_speed);

    const vec3 angular_velocity =
        PxToGlm(vehicle_.mBaseState.rigidBodyState.angularVelocity);
    gui::ViewProperty("Angular Velocity", angular_velocity);
}

void VehicleComponent::OnDestroy()
{
    physics_service_->UnregisterVehicle(&vehicle_, &GetEntity());
    vehicle_.destroy();
}

/* ----- EventSubscriber ----- */

void VehicleComponent::OnUpdate(const Timestep& delta_time)
{
    if (input_service_->IsKeyPressed(GLFW_KEY_F10))
    {
        LoadParams();
        debug::LogInfo("Reloaded vehicle params from JSON files...");
    }

    if (time_since_last_particle_ >= exhaust_delay_)
    {
        const vec3 particle_pos_left =
            transform_->GetPosition() +
            transform_->GetRightDirection() * kExhaustParticleOffset.x +
            transform_->GetUpDirection() * kExhaustParticleOffset.y +
            transform_->GetForwardDirection() * kExhaustParticleOffset.z;
        const vec3 particle_pos_right =
            transform_->GetPosition() +
            -transform_->GetRightDirection() * kExhaustParticleOffset.x +
            transform_->GetUpDirection() * kExhaustParticleOffset.y +
            transform_->GetForwardDirection() * kExhaustParticleOffset.z;

        exhaust_particles_->Emit(particle_pos_left);
        exhaust_particles_->Emit(particle_pos_right);

        time_since_last_particle_ = 0;
    }
    else
    {
        time_since_last_particle_ +=
            static_cast<float>(delta_time.GetSeconds());
    }

    const float speed_t = GetWheelSpeed() / 400.0f;
    exhaust_delay_ =
        glm::mix(kExhaustParticleDelayMin, kExhaustParticleDelayMax, speed_t);

    audio_emitter_->SetPitch(kDrivingAudio, GetDrivePitch());

    HandleVehicleTransform();
    UpdateGrounded();
    CheckAutoRespawn(delta_time);

    // We need it for the follow camera
    // get the player state
    if (GetEntity().HasComponent<PlayerState>())
    {
        jss::object_ptr<PlayerState> player_state =
            &GetEntity().GetComponent<PlayerState>();
        player_state->SetCurrentSpeed(GetSpeed());
    }
}

void VehicleComponent::OnPhysicsUpdate(const Timestep& step)
{
}

/* ----- Vehicle Functions ----- */

void VehicleComponent::LoadParams()
{
    const bool success_base = readBaseParamsFromJsonFile(
        kVehicleDataPath, kBaseParamFileName, vehicle_.mBaseParams);
    ASSERT_MSG(success_base,
               "Must be able to load vehicle base params from JSON file");

    const bool success_drivertrain = readDirectDrivetrainParamsFromJsonFile(
        kVehicleDataPath, kDirectDriveParamFileName,
        vehicle_.mBaseParams.axleDescription, vehicle_.mDirectDriveParams);
    ASSERT_MSG(success_drivertrain,
               "Must be able to load vehicle drivetrain params from JSON file");

    const bool success_integration = readPhysxIntegrationParamsFromJsonFile(
        kVehicleDataPath, kIntegrationParamFileName, vehicle_.mPhysXParams);
    ASSERT_MSG(
        success_integration,
        "Must be able to load vehicle integration params from JSON file");

    setPhysXIntegrationParams(vehicle_.mBaseParams.axleDescription,
                              gPhysXMaterialFrictions_,
                              gNbPhysXMaterialFrictions_,
                              kDefaultMaterialFriction, vehicle_.mPhysXParams);
}

void VehicleComponent::InitVehicle()
{
    const bool vehicle_init_status = vehicle_.initialize(
        *physics_service_->GetKPhysics(), PxCookingParams(PxTolerancesScale()),
        *physics_service_->GetKMaterial(), true);
    ASSERT_MSG(vehicle_init_status, "Vehicle must successfully initialize");

    vehicle_.mTransmissionCommandState.gear =
        PxVehicleDirectDriveTransmissionCommandState::eFORWARD;

    PxRigidBody* rigidbody = vehicle_.mPhysXState.physxActor.rigidBody;
    ASSERT_MSG(rigidbody, "Vehicle must have valid PhysX Actor RigidBody");

    rigidbody->userData = &GetEntity();
    rigidbody->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
    const uint32_t num_shapes = rigidbody->getNbShapes();
    PxShape* shape = nullptr;

    // First shape is the vehicle body, the next 4 should be the wheels.
    for (uint32_t i = 0; i < num_shapes; i++)
    {
        rigidbody->getShapes(&shape, 1, i);
        ASSERT_MSG(shape, "RigidBody Shape must be valid");

        if (i == 0)
        {
            // TODO: enabling collision for all shapes makes the vehicle get
            // stuck in the floor?
            shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
            shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        }

        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
        shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
    }

    // setting the max speed to be 200;
    vehicle_.mPhysXState.physxActor.rigidBody->setMaxLinearVelocity(
        max_velocity_);
}

void VehicleComponent::InitMaterialFrictionTable()
{
    // Each physx material can be mapped to a tire friction value on a per tire
    // basis. If a material is encountered that is not mapped to a friction
    // value, the friction value used is the specified default value. In this
    // snippet there is only a single material so there can only be a single
    // mapping between material and friction. In this snippet the same mapping
    // is used by all tires.
    gPhysXMaterialFrictions_[0].friction = 75.0f;
    gPhysXMaterialFrictions_[0].material = physics_service_->GetKMaterial();
    gNbPhysXMaterialFrictions_ = 1;
}

void VehicleComponent::HandleVehicleTransform()
{
    if (game_state_service_->GetRespawnRequested(this->GetEntity().GetId()))
    {
        // Adding 4.f in y axis, so that when the car respawns, if it is
        // oriented at a weird angle, it just doesnt go inside the track and be
        // half cut lol
        vehicle_.mPhysXState.physxActor.rigidBody->setGlobalPose(
            CreatePxTransform(
                transform_->GetPosition() + glm::vec3(0.0f, 16.5f, 0.0f),
                transform_->GetOrientation()));

        // now remove this from the list in gameservice so that it doesnt
        // respawn again and again until requested again later.
        game_state_service_->RemoveRespawnPlayers(this->GetEntity().GetId());

        // set the velocity and wheel rotation  of this car to be 0
        vehicle_.mBaseState.rigidBodyState.linearVelocity = physx::PxVec3(0.f);
        vehicle_.mBaseState.wheelRigidBody1dStates->rotationSpeed = 0.f;
    }

    else
    {
        vehicle_.mBaseState.rigidBodyState.linearVelocity =
            physx::PxVec3(max_velocity_);

        const PxTransform& pose =
            vehicle_.mPhysXState.physxActor.rigidBody->getGlobalPose();

        const GlmTransform transform = PxToGlm(pose);
        transform_->SetPosition(transform.position);
        transform_->SetOrientation(transform.orientation);
    }
}

void VehicleComponent::UpdateGrounded()
{
    // get orientation
    glm::vec3 position = transform_->GetPosition();
    glm::vec3 down = -transform_->GetUpDirection();

    // check
    auto raycast_data = physics_service_->RaycastStatic(position, down, 10.0f);
    is_grounded_ = raycast_data.has_value();
}

void VehicleComponent::Respawn()
{
    glm::vec3 last_checkpoint_pos;
    glm::vec3 next_checkpoint_pos;

    int current_checkpoint = game_state_service_->GetCurrentCheckpoint(
        GetEntity().GetId(), last_checkpoint_pos, next_checkpoint_pos);

    // entity wasn't found, so probably shouldn't try to respawn
    if (current_checkpoint == -1)
        return;

    // move vehicle to last checkpoint + orient towards next checkpoint
    transform_->SetPosition(last_checkpoint_pos);
    UpdateRespawnOrientation(next_checkpoint_pos, last_checkpoint_pos);
    game_state_service_->AddRespawnPlayers(GetEntity().GetId());

    // play respawn sound
    audio_emitter_->PlaySource(kRespawnAudio);

    debug::LogDebug("Entity {} respawned!", GetEntity().GetId());
}

void VehicleComponent::UpdateRespawnOrientation(
    const glm::vec3& next_checkpoint, const glm::vec3& last_checkpoint)
{
    auto current_orientation = transform_->GetOrientation();

    // assume car is initially oriented along the negative z-axis
    glm::vec3 forward = transform_->GetForwardDirection();
    glm::vec3 up = transform_->GetUpDirection();

    glm::vec3 direction = glm::normalize(next_checkpoint - last_checkpoint);
    glm::vec3 axis = glm::normalize(glm::cross(forward, direction));
    float angle = glm::acos(glm::dot(forward, direction));

    // face towards next checkpoint
    transform_->SetOrientation(glm::angleAxis(angle, axis) *
                               current_orientation);

    // flip via y axis if car is upside down
    if (up.y < 0.0f)
    {
        transform_->SetOrientation(transform_->GetOrientation() *
                                   -transform_->GetUpDirection());
    }
}

void VehicleComponent::CheckAutoRespawn(const Timestep& delta_time)
{
    if (is_grounded_)  // vehicle already grounded, no need to respawn
    {
        respawn_timer_ = 0.0f;
        return;
    }

    // increment timer
    respawn_timer_ += delta_time.GetSeconds();

    // when respawn time is up
    if (respawn_timer_ >= kRespawnSeconds)
    {
        Respawn();
        respawn_timer_ = 0.0f;
        return;
    }
}

/* ----- Setters + Getters ----- */

void VehicleComponent::SetMaxVelocity(float vel)
{
    max_velocity_ = vel;
    vehicle_.mPhysXState.physxActor.rigidBody->setMaxLinearVelocity(
        max_velocity_);
}

void VehicleComponent::SetMaxAchievableVelocity(float max_velocity)
{
    vehicle_.mPhysXState.physxActor.rigidBody->setMaxLinearVelocity(
        max_velocity);
}

void VehicleComponent::SetVehicleName(const string& vehicle_name)
{
    const PxTransform pose = CreatePxTransform(transform_->GetPosition(),
                                               transform_->GetOrientation());

    g_vehicle_name_ = vehicle_name;
    vehicle_.setUpActor(*physics_service_->GetKScene(), pose,
                        g_vehicle_name_.c_str());
}

void VehicleComponent::SetGear(VehicleGear gear)
{
    // this is an UNjustifiably long struct name lmao
    using enum PxVehicleDirectDriveTransmissionCommandState::Enum;

    PxVehicleDirectDriveTransmissionCommandState::Enum px_gear;
    switch (gear)
    {
        case VehicleGear::kReverse:
            px_gear = eREVERSE;
            break;

        case VehicleGear::kNeutral:
            px_gear = eNEUTRAL;
            break;

        case VehicleGear::kForward:
            px_gear = eFORWARD;
            break;
    }

    vehicle_.mTransmissionCommandState.gear = px_gear;
}

void VehicleComponent::SetCommand(VehicleCommand command)
{
    vehicle_.mCommandState.throttle = glm::clamp(command.throttle, 0.0f, 1.0f);
    vehicle_.mCommandState.steer = glm::clamp(command.steer, -1.0f, 1.0f);

    vehicle_.mCommandState.nbBrakes = 2;
    vehicle_.mCommandState.brakes[0] =
        glm::clamp(command.front_brake, 0.0f, 1.0f);
    vehicle_.mCommandState.brakes[1] =
        glm::clamp(command.rear_brake, 0.0f, 1.0f);
}

// to adjust the throttle for AIs.
float VehicleComponent::GetAdjustedSpeedMultiplier()
{
    return speed_adjuster_;
}

DirectDriveVehicle& VehicleComponent::GetVehicle()
{
    return vehicle_;
}

VehicleGear VehicleComponent::GetGear() const
{
    using enum PxVehicleDirectDriveTransmissionCommandState::Enum;
    switch (vehicle_.mTransmissionCommandState.gear)
    {
        case eREVERSE:
            return VehicleGear::kReverse;

        case eNEUTRAL:
            return VehicleGear::kNeutral;

        case eFORWARD:
            return VehicleGear::kForward;

        default:
            ASSERT_ALWAYS(
                "Vehicle transmission should never be in an "
                "unkown state");
            return VehicleGear::kNeutral;
    }
}

float VehicleComponent::GetSpeed() const
{
    const vec3 velocity =
        PxToGlm(vehicle_.mPhysXState.physxActor.rigidBody->getLinearVelocity());
    return glm::length(velocity);
}

float VehicleComponent::GetWheelSpeed() const
{
    return abs(vehicle_.mBaseState.wheelRigidBody1dStates->rotationSpeed);
}

float VehicleComponent::GetDrivePitch() const
{
    return glm::clamp(0.5f + GetWheelSpeed() / 400.0f, 0.0f, 1.2f);
}

bool VehicleComponent::IsGrounded() const
{
    return is_grounded_;
}
