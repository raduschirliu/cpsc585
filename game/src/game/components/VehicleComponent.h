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
    /**
     *  resets the vehicle's position to their previously hit checkpoint,
     *  oriented towards the next checkpoint to be hit
     */
    void Respawn();

    /* ----- From Component ----- */

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;
    void OnDebugGui() override;
    void OnDestroy() override;

    /* ----- Event subscribers ----- */

    void OnUpdate(const Timestep& delta_time) override;
    void OnPhysicsUpdate(const Timestep& step) override;

    /* ----- Setters + Getters ----- */

    void SetCommand(VehicleCommand command);
    void SetVehicleName(const std::string& vehicle_name);
    void SetGear(VehicleGear gear);
    void SetMaxVelocity(float vel);
    void SetMaxAchievableVelocity(float max_velocity);

    snippetvehicle2::DirectDriveVehicle& GetVehicle();
    VehicleGear GetGear() const;
    float GetSpeed() const;
    float GetAdjustedSpeedMultiplier();
    bool IsGrounded() const;

  private:
    void InitVehicle();
    void InitMaterialFrictionTable();
    void LoadParams();
    void HandleVehicleTransform();
    void UpdateGrounded();
    /// @brief respawns vehicle when not grounded for an amount of time
    /// @see respawn_timer_
    void CheckAutoRespawn(const Timestep& delta_time);
    /// orients the vehicle towards next checkpoint on respawn
    void UpdateRespawnOrientation(const glm::vec3& next_checkpoint,
                                  const glm::vec3& checkpoint);

    /// The vehicle with direct drivetrain
    snippetvehicle2::DirectDriveVehicle vehicle_;
    std::string g_vehicle_name_;
    /// The mapping between PxMaterial and friction.
    physx::vehicle2::PxVehiclePhysXMaterialFriction
        gPhysXMaterialFrictions_[16];
    physx::PxU32 gNbPhysXMaterialFrictions_ = 0;

    /// i.e. Whether or not the vehicle is midair or upside down
    bool is_grounded_;
    /// how long until an ungrounded vehicle is respawned
    float respawn_timer_;
    float speed_adjuster_;
    float max_velocity_ = 130.0f;

    /* ----- Service and Component dependencies ----- */

    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<GameStateService> game_state_service_;

    jss::object_ptr<Transform> transform_;
    jss::object_ptr<AudioEmitter> audio_emitter_;
};
