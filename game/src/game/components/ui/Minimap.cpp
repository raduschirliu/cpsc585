#include <game/components/ui/Minimap.h>

#include <glm/gtx/transform.hpp>

#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/input/InputService.h"
#include "engine/render/Camera.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"
#include "game/components/VehicleComponent.h"

using namespace glm;
using glm::quat;
using glm::vec3;

std::string_view Minimap::GetName() const
{
    return "Minimap";
}

void Minimap::OnInit(const ServiceProvider& service_provider)
{
    // Dependencies
    transform_ = &GetEntity().GetComponent<Transform>();

    // Event subscriptions
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void Minimap::SetFollowingTransform(Entity& entity)
{
    target_transform_ = &entity.GetComponent<Transform>();
    target_vehicle_ = &entity.GetComponent<VehicleComponent>();
}