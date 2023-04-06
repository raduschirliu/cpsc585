#include "Pickup.h"

#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"
static constexpr float kRotationSpeed = 90.0f;

void Pickup::OnInit(const ServiceProvider& service_provider)
{
    transform_ = &GetEntity().GetComponent<Transform>();
    game_state_ = &service_provider.GetService<GameStateService>();
    pickup_service_ = &service_provider.GetService<PickupService>();
    if (game_state_)
    {
        k_player_names_ = game_state_->GetPlayerStaticNames();
    }

    if (pickup_service_)
    {
        ammo_types_ = pickup_service_->GetAmmoPickupNames();
        powerup_types_ = pickup_service_->GetPowerupPickupNames();
    }

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void Pickup::OnStart()
{
}

void Pickup::OnTriggerEnter(const OnTriggerEvent& data)
{
}

void Pickup::OnUpdate(const Timestep& delta_time)
{
    // rotate the powerup around its y axis.
    // TODO: Ask Radu to help with rotating the powerups every frame.

    // Rotate by 90 degrees/sec
    transform_->RotateEulerDegrees(
        glm::vec3(kRotationSpeed * delta_time.GetSeconds(), 0.0f, 0.0f));
}

std::string_view Pickup::GetName() const
{
    return "";
}

void Pickup::OnTriggerExit(const OnTriggerEvent& data)
{
}

void Pickup::SetPowerVisibility(bool bValue)
{
    power_visible_ = bValue;
}

void Pickup::SetVehiclePowerup(PowerupPickupType type,
                               const OnTriggerEvent& data)
{
    player_state_ = &data.other->GetComponent<PlayerState>();
    if (player_state_)
    {
        // storing the powerup in the playerstate struct
        player_state_->SetCurrentPowerup(type);
    }
}
