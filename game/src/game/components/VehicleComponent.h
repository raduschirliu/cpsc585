#pragma once

#include <physx/CommonVehicleFiles/directdrivetrain/DirectDrivetrain.h>

#include <object_ptr.hpp>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdPhysx.h"
#include "engine/fwd/FwdServices.h"
#include "engine/physics/OnPhysicsUpdateEvent.h"
#include "engine/physics/VehicleCommands.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/FwdGame.h"
#include "game/components/audio/AudioEmitter.h"

class VehicleComponent final : public Component,
                               public IEventSubscriber<OnUpdateEvent>,
                               public IEventSubscriber<OnPhysicsUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;
    void OnDebugGui() override;
    void OnDestroy() override;

    // Event subscribers
    void OnUpdate(const Timestep& delta_time) override;
    void OnPhysicsUpdate(const Timestep& step) override;

    // Getters and Setters
    void SetCommand(VehicleCommand command);
    void SetPlayerStateData(PlayerStateData& data);
    void SetVehicleName(const std::string& vehicle_name);
    void SetGear(VehicleGear gear);
    void SetMaxVelocity(float vel);
    void SetMaxAchievableVelocity(float max_velocity);

    snippetvehicle2::DirectDriveVehicle& GetVehicle();
    VehicleGear GetGear() const;
    float GetSpeed() const;
    float GetAdjustedSpeedMultiplier();
    bool IsGrounded() const;

    void Respawn();

  private:
    void InitVehicle();
    void InitMaterialFrictionTable();
    void LoadParams();
    void HandleVehicleTransform();
    void UpdateGrounded();

    /// @brief respawns vehicle when not grounded for an amount of time
    /// @see respawn_timer_
    void CheckAutoRespawn(const Timestep& delta_time);
    
    /// respawns vehicle when all health is depleted
    void CheckDeathRespawn();

    void UpdateRespawnOrientation(const glm::vec3& next_checkpoint,
                                  const glm::vec3& checkpoint);

    /// The mapping between PxMaterial and friction.
    physx::vehicle2::PxVehiclePhysXMaterialFriction
        gPhysXMaterialFrictions_[16];
    physx::PxU32 gNbPhysXMaterialFrictions_ = 0;

    /// i.e. Whether or not the vehicle is midair or upside down
    bool is_grounded_;
    float respawn_timer_;

    std::string g_vehicle_name_;
    PlayerStateData* player_data_;

    float speed_adjuster_;
    float max_velocity_ = 130.f;

    // Service and Component dependencies
    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<GameStateService> game_state_service_;
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<AudioEmitter> audio_emitter_;
    /// The vehicle with direct drivetrain
    snippetvehicle2::DirectDriveVehicle vehicle_;
};
