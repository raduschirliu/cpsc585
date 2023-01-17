#include "engine/scene/ComponentBuilder.h"

ComponentBuilder::ComponentBuilder(ServiceProvider& service_provider,
                                   EventBus& event_bus)
    : service_provider_(service_provider),
      event_bus_(event_bus)
{
}