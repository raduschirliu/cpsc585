#include "Shooter.h"

#include <array>
#include <random>

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "game/services/GameStateService.h"

static float RandomPitchValue();  // TODO: move this to AudioService
static constexpr float kBaseDamage = 10.0f;

// the buckshot will have 10 different pellets from the barrel
static constexpr uint8_t kNumberPellets = 10;
static constexpr double kMaxTimer = 4.0f;

void Shooter::Shoot()
{
    // origin and direction of the raycast from this entity
    glm::vec3 fwd_direction = transform_->GetForwardDirection();
    glm::vec3 offset = (hitbox_->GetSize() + 5.0f) * fwd_direction;
    glm::vec3 origin = transform_->GetPosition() + offset;

    current_ammo_type_ = player_state_->GetCurrentAmmoType();

    // play shoot sound; slightly randomize pitch
    if (!game_state_service_->GetDisplayPauseBoolean())
    {
        audio_emitter_->SetPitch(shoot_sound_file_, RandomPitchValue());
        audio_emitter_->PlaySource(shoot_sound_file_);
    }

    if (current_ammo_type_ == AmmoPickupType::kBuckshot)
    {
        // shotgun type bullet, spread.
        ShootBuckshot(origin, fwd_direction);
        return;
    }
    else if (current_ammo_type_ == AmmoPickupType::kDoubleDamage)
    {
        ShootDoubleDamage(origin, fwd_direction);
        return;
    }
    else if (current_ammo_type_ == AmmoPickupType::kExploadingBullet)
    {
        ShootExploading(origin, fwd_direction);
        return;
    }
    else if (current_ammo_type_ == AmmoPickupType::kIncreaseFireRate)
    {
        IncreaseFireRate();
        return;
    }
    else if (current_ammo_type_ == AmmoPickupType::kVampireBullet)
    {
        ShootVampire(origin, fwd_direction);
        return;
    }
    else
    {
        ShootDefault(origin, fwd_direction);
    }
}

void Shooter::ShootDefault(const glm::vec3& origin,
                           const glm::vec3& fwd_direction)
{
    // check if shot hit anything
    target_data_ = physics_service_->Raycast(origin, fwd_direction);
    if (target_data_ && !target_data_.has_value())
    {
        uint32_t entity_id = GetEntity().GetId();
        return;
    }
    else
    {
    }

    // get the entity that was hit
    if (target_data_)
    {
        RaycastData target_data_value = target_data_.value();
        Entity* target_entity = target_data_value.entity;

        UpdateOnHit();
    }
}

void Shooter::ShootBuckshot(const glm::vec3& origin,
                            const glm::vec3& fwd_direction)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    std::vector<std::optional<RaycastData>> target_datas;
    // as we want the shots to scatter
    for (int i = 0; i < kNumberPellets; i++)
    {
        std::array<float, 3> spread = {dis(gen) / 4.0f, dis(gen) / 4.0f,
                                       dis(gen) / 4.0f};
        target_data_ = physics_service_->Raycast(
            origin, glm::normalize(fwd_direction +
                                   glm::vec3(spread[0], 0.0f, spread[2])));
        if (!target_data_ && !target_data_.has_value())
        {
            // do nothing as this raycast failed...
        }
        else
        {
            // add this value to the target_datas.
            target_datas.push_back(target_data_);
        }
    }

    if (target_datas.size() == 0 || !target_datas[0].has_value())
    {
        return;
    }

    Entity* target_entity = target_datas[0].value().entity;

    if (!target_entity->HasComponent<PlayerState>())
    {
        return;
    }

    jss::object_ptr<PlayerState> target_state =
        &target_entity->GetComponent<PlayerState>();

    // TODO: fix this, as the pellets spread, they can hit multiple car, rn we
    // apply the hit to only one car. FIX THIS as this many pellets hit the car.
    target_state->DecrementHealth(GetAmmoDamage() * target_datas.size());
}

/// @brief handles the vampire bullets
void Shooter::ShootVampire(const glm::vec3& origin,
                           const glm::vec3& fwd_direction)
{
    // check if shot hit anything
    target_data_ = physics_service_->Raycast(origin, fwd_direction);
    // get the entity that was hit
    if (target_data_.has_value())
    {
        RaycastData target_data_value = target_data_.value();
        Entity* target_entity = target_data_value.entity;
        UpdateOnHit();
    }
}

/// @brief handles the double damage bullets
void Shooter::ShootDoubleDamage(const glm::vec3& origin,
                                const glm::vec3& fwd_direction)
{
    // check if shot hit anything
    target_data_ = physics_service_->Raycast(origin, fwd_direction);
    // get the entity that was hit
    if (target_data_.has_value())
    {
        RaycastData target_data_value = target_data_.value();
        Entity* target_entity = target_data_value.entity;

        UpdateOnHit();
    }
}

/// @brief handles the exploading damage bullets
void Shooter::ShootExploading(const glm::vec3& origin,
                              const glm::vec3& fwd_direction)
{
    // check if shot hit anything
    target_data_ = physics_service_->Raycast(origin, fwd_direction);
    // get the entity that was hit
    if (target_data_.has_value())
    {
        RaycastData target_data_value = target_data_.value();
        Entity* target_entity = target_data_value.entity;
        UpdateOnHit();
    }
}

/// @brief handles the increase fire rate
void Shooter::IncreaseFireRate()
{
    // doubling the bullets speed which can be fired at a time.
    increase_fire_speed_multiplier_ = 0.5f;
}

// The duration between which the 2 consecutive bullets will shoot.
float Shooter::GetCooldownTime()
{
    using enum AmmoPickupType;
    switch (current_ammo_type_)
    {
        case kIncreaseFireRate:
            return 0.5f;
            break;
        case kExploadingBullet:
            return 2.5f;
            break;
        case kBuckshot:
            return 1.5f;
            break;
        case kDoubleDamage:
            return 2.0f;
            break;
        case kVampireBullet:
            return 2.0f;
            break;
        default:
            return 1.0f * increase_fire_speed_multiplier_;
            break;
    }
}

void Shooter::UpdateOnHit()
{
    if (target_data_)
    {
        Entity* target_entity = target_data_.value().entity;

        if (!target_entity->HasComponent<PlayerState>())
        {
            return;
        }

        jss::object_ptr<PlayerState> target_state =
            &target_entity->GetComponent<PlayerState>();

        target_state->DecrementHealth(GetAmmoDamage());

        // vampire bullet increases own players health
        if (current_ammo_type_ == AmmoPickupType::kVampireBullet)
        {
            player_state_->IncrementHealth(GetAmmoDamage());
        }
    }
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
    audio_emitter_->AddSource(shoot_sound_file_);
    audio_emitter_->SetGain(shoot_sound_file_, 0.2f);
}

float Shooter::GetAmmoDamage()
{
    using enum AmmoPickupType;

    float damage_multiplier;
    switch (current_ammo_type_)
    {
        case kDoubleDamage:
            damage_multiplier = 2.0f;
            break;
        case kBuckshot:
            damage_multiplier = 0.5f;
            break;
        case kExploadingBullet:
            damage_multiplier = 2.5f;
            break;
        case kVampireBullet:
            damage_multiplier = 0.5f;
            break;
        default:
            damage_multiplier = 1.0f;
    }
    return damage_multiplier * kBaseDamage;
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
    game_state_service_ = &service_provider.GetService<GameStateService>();
    physics_service_ = &service_provider.GetService<PhysicsService>();
    audio_service_ = &service_provider.GetService<AudioService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();
    hitbox_ = &GetEntity().GetComponent<Hitbox>();
    player_state_ = &GetEntity().GetComponent<PlayerState>();
    audio_emitter_ = &GetEntity().GetComponent<AudioEmitter>();

    // set initial shoot sound
    shoot_sound_file_ = "kart_shoot_01.ogg";
    audio_emitter_->AddSource(shoot_sound_file_);
    audio_emitter_->SetGain(shoot_sound_file_, 0.2f);

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
        // so that timer starts only once.
        if (timer_.find(current_ammo_type_) == timer_.end())
        {
            timer_.insert_or_assign(current_ammo_type_, 0.0f);
        }
        SetShootSound(current_ammo_type_);
    }

    for (auto& timer : timer_)
    {
        if (timer.second <= kMaxTimer)
        {
            timer.second += delta_time.GetSeconds();
            continue;
        }
        else
        {
            // reset the powerup to default
            if (player_state_)
            {
                player_state_->SetCurrentAmmoType(AmmoPickupType::kDefaultAmmo);
            }
        }
    }
}