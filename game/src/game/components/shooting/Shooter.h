#pragma once

#include <optional>

#include "engine/fwd/FwdComponents.h"
#include "engine/fwd/FwdServices.h"
#include "engine/physics/RaycastData.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "game/components/audio/AudioEmitter.h"
#include "game/components/shooting/Hitbox.h"
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
     *  get the amount of cooldown time inbetween shots.
     *  cooldown length may differ depending on the type of ammo
     */
    Timestep GetCooldownTime();

    /* ----- from component ----- */

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    /* ----- from IEventSubscriber ----- */

    void OnUpdate(const Timestep& delta_time) override;

  private:
    /// hits multiple opponents in some range
    void ShootBuckshot();
    /// updates the target that was hit (health, etc.)
    void UpdateOnHit();
    /// sets the sound of the shot depending on the ammo type
    void SetShootSound(AmmoPickupType ammo_type);
    float GetAmmoDamage();

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
};