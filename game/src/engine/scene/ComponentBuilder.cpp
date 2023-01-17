#include "engine/scene/ComponentBuilder.h"

ComponentBuilder::ComponentBuilder(ServiceProvider& service_provider,
                                   GlobalEventDispatcher& event_dispatcher)
    : service_provider_(service_provider),
      event_dispatcher_(event_dispatcher)
{
}