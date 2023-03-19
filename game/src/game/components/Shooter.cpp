#include "game/components/Shooter.h"

#include <glm/glm.hpp>
#include <object_ptr.hpp>
#include <random>

#include "PxPhysics.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/physics/BoxRigidBody.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/RaycastData.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"
#include "game/components/audio/AudioEmitter.h"
#include "game/components/state/PlayerState.h"

using glm::vec3;

static constexpr size_t kGamepadId = GLFW_JOYSTICK_1;

void Shooter::Shoot()
{
    // origin and direction of the raycast from this entity
    vec3 direction = transform_->GetForwardDirection();
    vec3 offset = (hitbox_->GetSize() + 10.0f) * direction;
    vec3 origin = transform_->GetPosition() + offset;

    current_ammo_type_ = player_state_->GetCurrentAmmoType();

    if (current_ammo_type_ == AmmoPickupType::kBuckshot)
    {
        ShootBuckshot();
        return;
    }

    audio_emitter_->AddSource(shoot_sound_file_);
    // slightly randomize pitch
    audio_emitter_->SetPitch(shoot_sound_file_, RandomPitchValue());
    audio_emitter_->PlaySource(shoot_sound_file_);

    target_data_ = physics_service_->Raycast(origin, direction);
    if (!target_data_.has_value())
    {
        std::uint32_t entity_id = GetEntity().GetId();
        Log::debug("Entity: {} did not hit anything.", entity_id);
        return;
    }

    // get the data from the raycast hit
    RaycastData target_data_value = target_data_.value();

    float distance = target_data_value.distance;
    physx::PxActor* target_actor = target_data_value.actor;
    vec3 normal = target_data_value.normal;
    vec3 position = target_data_value.position;

    std::uint32_t entity_id = GetEntity().GetId();
    Log::debug("Entity: {} hit something!", entity_id);
}

std::optional<RaycastData> Shooter::GetTargetData()
{
    return target_data_;
}

void Shooter::ShootBuckshot()
{
    Log::debug("NOT IMPLEMENTED YET OOOOPS");
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

/* ----- from component ----- */

void Shooter::OnInit(const ServiceProvider& service_provider)
{
    Log::info("{} Component - Init", GetName());

    // service dependencies
    physics_service_ = &service_provider.GetService<PhysicsService>();
    input_service_ = &service_provider.GetService<InputService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();
    hitbox_ = &GetEntity().GetComponent<Hitbox>();
    player_state_ = &GetEntity().GetComponent<PlayerState>();
    audio_emitter_ = &GetEntity().GetComponent<AudioEmitter>();

    shoot_sound_file_ = "kart_shoot_01.ogg";

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

    if (input_service_->IsKeyPressed(GLFW_KEY_R) ||
        input_service_->IsGamepadButtonPressed(kGamepadId,
                                               GLFW_GAMEPAD_BUTTON_B))
    {
        Shoot();
    }
}

float RandomPitchValue()
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> value(0, 500);

    int coefficient;
    if (value(generator) >= 250)
    {
        coefficient = -1;
    }
    else
    {
        coefficient = 1;
    }

    return 1 + coefficient * (value(generator) / 1000.0f);
}
