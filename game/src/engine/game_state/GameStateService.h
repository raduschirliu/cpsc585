#pragma once

#include <map>
#include <memory>
#include <object_ptr.hpp>
#include <set>
#include <utility>  // for pair
#include <vector>

#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/service/Service.h"
#include "game/components/Pickups/PickupType.h"
#include "game/components/state/PlayerStateStruct.h"

class PlayerState;

class GameStateService : public Service, public IEventSubscriber<OnUpdateEvent>
{
  public:
    GameStateService();

    // From Service
    void OnInit() override;
    void OnUpdate() override;
    void OnUpdate(const Timestep& delta_time) override;
    void OnCleanup() override;
    void OnStart(ServiceProvider& service_provider);
    std::string_view GetName() const override;

  private:
    std::map<uint32_t, PlayerStateData> player_details_;
    std::map<uint32_t, PlayerState*> player_states_;
    std::map<uint32_t, PowerupPickupType> player_powers_;
    std::set<std::pair<uint32_t, PowerupPickupType>> same_powerup_;

    std::vector<std::pair<uint32_t, PowerupPickupType>> active_powerups_;

    std::map<std::pair<uint32_t, PowerupPickupType>, float> timer_;

    void CheckTimer(double timer_limit, PowerupPickupType pickup_type);

  public:
    // setters
    void AddPlayerDetails(uint32_t id, PlayerStateData details);
    void AddPlayerStates(uint32_t id, PlayerState* states);
    void AddPlayerPowerup(uint32_t id, PowerupPickupType power);
    void RemovePlayerPowerup(uint32_t id);

    std::vector<std::pair<uint32_t, PowerupPickupType>> PowerupsActive();

    std::vector<std::pair<uint32_t, PowerupPickupType>> GetActivePowerups();

    uint32_t GetEveryoneSlowerSpeedMultiplier();
    uint32_t GetDisableHandlingMultiplier();
    uint32_t GetHitBoxMultiplier();

    void RemoveEveryoneSlowerSpeedMultiplier();
    void RemoveActivePowerup();
};