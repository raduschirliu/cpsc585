#pragma once

#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"
#include "game/services/GameStateService.h"

class Checkpoint final : public Component
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnTriggerEnter(const OnTriggerEvent& data) override;
    std::string_view GetName() const override;

    void SetCheckpointIndex(int index);
    int GetCheckpointIndex() const;

  private:
    jss::object_ptr<GameStateService> game_service_;
    int checkpoint_index_;
};
