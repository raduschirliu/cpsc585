#pragma once

#include <object_ptr.hpp>

#include "engine/input/InputService.h"
#include "engine/physics/OnPhysicsUpdateEvent.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/VehicleCommands.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/audio/SoundEmitter.h"  // debugging
#include "game/services/GameStateService.h"
#include "vehicle2/PxVehicleAPI.h"

class VehicleComponent final : public Component,
                               public IEventSubscriber<OnUpdateEvent>,
                               public IEventSubscriber<OnPhysicsUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDestroy() override;
    void OnDebugGui() override;
    std::string_view GetName() const override;

    // Event subscribers
    void OnUpdate(const Timestep& delta_time) override;
    void OnPhysicsUpdate(const Timestep& step) override;

    void SetGear(VehicleGear gear);
    void SetCommand(VehicleCommand command);

    // Accessors
    void SetVehicleName(const std::string& vehicle_name);
    void SetPlayerStateData(PlayerStateData& data);

    snippetvehicle2::DirectDriveVehicle& GetVehicle();

    VehicleGear GetGear() const;
    float GetSpeed() const;
    //void SetMaxSpeed(float max_velocity) const;

  private:
    jss::object_ptr<SoundEmitter> sound_emitter_;  // debugging
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<GameStateService> game_state_service_;

    // The vehicle with direct drivetrain
    snippetvehicle2::DirectDriveVehicle vehicle_;

    // The mapping between PxMaterial and friction.
    physx::vehicle2::PxVehiclePhysXMaterialFriction
        gPhysXMaterialFrictions_[16];
    physx::PxU32 gNbPhysXMaterialFrictions_ = 0;

    std::string g_vehicle_name_;

    PlayerStateData* player_data_;

    void InitVehicle();
    void InitMaterialFrictionTable();
    void LoadParams();
};
