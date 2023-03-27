#pragma once

#include <optional>

#include "engine/audio/AudioService.h"
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
     *  emits a raycast shot from the entity, updating the entity it hits
     *  (if it hits)
     */
    void Shoot();

    /**
     *  get the amount of cooldown time inbetween shots.
     *  cooldown length may differ depending on the type of ammo
     */
    float GetCooldownTime();

    /* ----- from component ----- */

    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    /* ----- from IEventSubscriber ----- */

    void OnUpdate(const Timestep& delta_time) override;

  private:
    /// hits multiple opponents in some range
    void ShootBuckshot(glm::vec3 fwd_direction);

    /// @brief handles the vampire bullets 
    void ShootVampire();

    /// @brief handles the double damage bullets
    void ShootDoubleDamage();

    /// @brief handles the exploading damage bullets
    void ShootExploading();

    /// @brief handles the increase fire rate
    void IncreaseFireRate();

    /// @brief handles shooting normal rounds.
    void ShootDefault(glm::vec3 origin, glm::vec3 fwd_dirction);

    /// updates the target that was hit (health, etc.)
    void UpdateOnHit();

    /// sets the sound of the shot depending on the ammo type
    void SetShootSound(AmmoPickupType ammo_type);
    /// gets the appropriate damage for the current ammo type
    float GetAmmoDamage();

    AmmoPickupType current_ammo_type_;
    std::optional<RaycastData> target_data_;
    std::string shoot_sound_file_;

    /* ----- service and component dependencies ----- */

    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<AudioService> audio_service_;

    jss::object_ptr<Transform> transform_;
    jss::object_ptr<Hitbox> hitbox_;
    jss::object_ptr<AudioEmitter> audio_emitter_;
    jss::object_ptr<PlayerState> player_state_;
};