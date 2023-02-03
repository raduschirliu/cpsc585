#pragma once

#include <object_ptr.hpp>

#include "engine/gui/GuiService.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/input/InputService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class GuiExampleComponent final : public Component,
                                  public IEventSubscriber<OnGuiEvent>,
                                  public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
};
