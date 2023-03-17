#include "game/components/Shooter.h"

#include <stdio.h>

#include <glm/glm.hpp>
#include <object_ptr.hpp>

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

    current_ammo_type_ = player_state_->GetCurrentAmmoType();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void Shooter::OnUpdate(const Timestep& delta_time)
{
    if (current_ammo_type_ != player_state_->GetCurrentAmmoType())
    {
        SetShootSound(player_state_->GetCurrentAmmoType());
    }

    /* if (input_service_->IsKeyPressed(GLFW_KEY_R) || */
    /*     input_service_->IsGamepadButtonPressed(kGamepadId, */
    /*                                            GLFW_GAMEPAD_BUTTON_B)) */
    /* { */
    /*     Shoot(); */
    /* } */
}

std::string_view Shooter::GetName() const
{
    return "Shooter";
}

void Shooter::Shoot()
{
    // origin and direction of the raycast from this entity
    vec3 direction = transform_->GetForwardDirection();
    vec3 offset = (hit_box_->GetSize() + 5.f) * direction;
    vec3 origin = transform_->GetPosition() + offset;

    target_data_ = physics_service_->Raycast(origin, direction);

    if (!target_data_.has_value())
    {
        return;
    }

    // get the data from the raycast hit
    RaycastData target_data_value = target_data_.value();

    float distance = target_data_value.distance;
    physx::PxActor* target_actor = target_data_value.actor;
    vec3 normal = target_data_value.normal;
    vec3 position = target_data_value.position;

    std::cout << "target actor: " << target_actor << "\n"
              << "target position: " << position << "\t"
              << "distance from target: " << distance << "\t"
              << "normal of raycast hit: " << normal << "\t" << std::endl;
}

RaycastData Shooter::GetTargetData()
{
    return target_data_;
}

void Shooter::SetShootSound(AmmoPickupType ammo_type)
{
    using enum AmmoPickupType;
    switch (ammo_type)
    {
        case kDoubleDamage:
            audio_emitter_->AddSource("kart_shoot_02.ogg");
            break;
        case kExploadingBullet:
            audio_emitter_->AddSource("kart_shoot_03.ogg");
            break;
        // TODO!! need to find sounds
        case kBuckshot:
        case kIncreaseFireRate:
        case kVampireBullet:
        default:
            audio_emitter_->AddSource("kart_shoot_01.ogg");
            break;
    }
}
