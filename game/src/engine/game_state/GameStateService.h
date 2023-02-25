#pragma once

#include <map>
#include <memory>
#include <object_ptr.hpp>
#include <vector>

#include "../../game/components/state/PlayerStateStruct.h"
#include "engine/scene/Entity.h"
#include "engine/service/Service.h"

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
    std::map<Entity*, PlayerStateStruct> player_details_;

  public:
    // setters
    inline void AddPlayerDetails(Entity* entity, PlayerStateStruct details)
    {
        player_details_.insert_or_assign(entity, details);
    }
};