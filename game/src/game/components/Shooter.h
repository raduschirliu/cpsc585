#pragma once

#include <optional>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/physics/Hitbox.h"
#include "engine/physics/RaycastData.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/components/audio/AudioEmitter.h"
#include "game/components/state/PlayerState.h"

class Shooter final : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    /**
     *  emits a raycast shot from the entity and stores the shot's data.
     *
     *  @see GetTargetData()
     */
    void Shoot();

    /**
     *  gets the data of the shot and the actor that was hit, its position,
     *  distance, etc. (may be null).
     *
     *  @return either std::null_opt or a struct containing all the shot data
     */
    std::optional<RaycastData> GetTargetData();

    /* ----- from component ----- */

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    /* ----- from IEventSubscriber ----- */

    void OnUpdate(const Timestep& delta_time) override;

  private:
    AmmoPickupType current_ammo_type_;
    std::optional<RaycastData> target_data_;
    std::string shoot_sound_file_;

    /* ----- service and component dependencies ----- */

    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<InputService> input_service_;

    jss::object_ptr<Transform> transform_;
    jss::object_ptr<Hitbox> hitbox_;
    jss::object_ptr<AudioEmitter> audio_emitter_;
    jss::object_ptr<PlayerState> player_state_;

    void ShootBuckshot();

    /// sets the sound of the shot depending on the ammo type
    void SetShootSound(AmmoPickupType ammo_type);
};