#pragma once

#include <map>
#include <memory>
#include <object_ptr.hpp>
#include <vector>

#include "../../game/components/state/PlayerStateStruct.h"
#include "engine/scene/Entity.h"
#include "engine/service/Service.h"
#include "../../game/components/Pickups/Pickup.h"

class GameStateService : public Service
{
  public:
    GameStateService();

    // From Service
    void OnInit() override;
    void OnUpdate() override;
    void OnCleanup() override;
    void OnStart(ServiceProvider& service_provider);
    std::string_view GetName() const override;

  private:
    std::map<uint32_t, PlayerStateData> player_details_;
    std::map<uint32_t, PowerupPickupType> player_powers_;

  public:
    // setters
    void AddPlayerDetails(uint32_t id, PlayerStateData details);

    void AddPlayerPowerup(uint32_t id, PowerupPickupType power);
    void RemovePlayerPowerup(uint32_t id);

    std::vector<PowerupPickupType> PowerupsActive();
};