#include "game/components/shooting/Shooter.h"

#include <random>

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"

static float RandomPitchValue();  // TODO: move this to AudioService

void Shooter::Shoot()
{
    // origin and direction of the raycast from this entity
    glm::vec3 direction = transform_->GetForwardDirection();
    glm::vec3 offset = (hitbox_->GetSize() + 5.0f) * direction;
    glm::vec3 origin = transform_->GetPosition() + offset;

    current_ammo_type_ = player_state_->GetCurrentAmmoType();

    if (current_ammo_type_ == AmmoPickupType::kBuckshot)
    {
        ShootBuckshot();
        return;
    }

    // play shoot sound
    audio_emitter_->AddSource(shoot_sound_file_);
    // slightly randomize pitch
    audio_emitter_->SetPitch(shoot_sound_file_, RandomPitchValue());
    audio_emitter_->PlaySource(shoot_sound_file_);

    // check if shot hit anything
    target_data_ = physics_service_->Raycast(origin, direction);
    if (!target_data_.has_value())
    {
        uint32_t entity_id = GetEntity().GetId();
        debug::LogDebug("Entity: {} did not hit anything.", entity_id);
        return;
    }

    // get the entity that was hit
    RaycastData target_data_value = target_data_.value();
    Entity* target_entity = target_data_value.entity;

    // update that entity accordingly
    UpdateOnHit();

    // debugggg
    uint32_t entity_id = GetEntity().GetId();
    debug::LogDebug("Entity: {} hit Entity {}!", entity_id,
                    target_entity->GetId());
}

void Shooter::ShootBuckshot()
{
    debug::LogDebug("NOT IMPLEMENTED YET OOOOPS");
}

// NOT FINAL IN THE SLIGHTEST
Timestep Shooter::GetCooldownTime()
{
    using enum AmmoPickupType;
    switch (current_ammo_type_)
    {
        case kIncreaseFireRate:
            return Timestep::Seconds(0.5f);
            break;
        case kBuckshot:
        case kDoubleDamage:
        case kExploadingBullet:
        case kVampireBullet:
            return Timestep::Seconds(2.0f);
            break;
        default:
            return Timestep::Seconds(1.0f);
            break;
    }
}

void Shooter::UpdateOnHit()
{
    Entity* target_entity = target_data_.value().entity;

    if (!target_entity->HasComponent<PlayerState>())
    {
        return;
    }

    jss::object_ptr<PlayerState> target_state =
        &target_entity->GetComponent<PlayerState>();

    target_state->DecrementHealth(GetAmmoDamage());
}

void Shooter::SetShootSound(AmmoPickupType ammo_type)
{
    using enum AmmoPickupType;
    switch (ammo_type)
    {
        case kDoubleDamage:
            shoot_sound_file_ = "kart_shoot_02.ogg";
            break;
        case kExploadingBullet:
            shoot_sound_file_ = "kart_shoot_03.ogg";
            break;
        // TODO!! need to find sounds
        case kBuckshot:
        case kIncreaseFireRate:
        case kVampireBullet:
        default:
            shoot_sound_file_ = "kart_shoot_01.ogg";
            break;
    }
}

float Shooter::GetAmmoDamage()
{
    using enum AmmoPickupType;
    float base_damage = 10.0f;
    switch (current_ammo_type_)
    {
        case kDoubleDamage:
            return 2.0f * base_damage;
            break;
        // TODO
        case kBuckshot:          // do less for each shot ?
        case kExploadingBullet:  // do a lot but slower ?
        case kVampireBullet:     // do less ??
        default:
            return base_damage;
    }
}

float RandomPitchValue()
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> value(0, 500);

    int coefficient = (value(generator) >= 250) ? -1 : 1;

    return 1 + coefficient * (value(generator) / 1000.0f);
}

/* ----- from component ----- */

void Shooter::OnInit(const ServiceProvider& service_provider)
{
    debug::LogInfo("{} Component - Init", GetName());

    // service dependencies
    physics_service_ = &service_provider.GetService<PhysicsService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();
    hitbox_ = &GetEntity().GetComponent<Hitbox>();
    player_state_ = &GetEntity().GetComponent<PlayerState>();
    audio_emitter_ = &GetEntity().GetComponent<AudioEmitter>();

    shoot_sound_file_ = "kart_shoot_01.ogg";
    hitbox_ = &GetEntity().GetComponent<Hitbox>();
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

std::string_view Shooter::GetName() const
{
    return "Shooter";
}

/* ----- from IEventSubscriber ----- */

void Shooter::OnUpdate(const Timestep& delta_time)
{
    if (current_ammo_type_ != player_state_->GetCurrentAmmoType())
    {
        current_ammo_type_ = player_state_->GetCurrentAmmoType();
        SetShootSound(current_ammo_type_);
    }
}