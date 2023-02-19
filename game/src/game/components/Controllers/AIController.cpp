#include "AIController.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

AIController::AIController()
    : input_service_(nullptr),
      transform_(nullptr),
      ai_service_(nullptr)
{
}

void AIController::OnInit(const ServiceProvider& service_provider)
{
    input_service_ = &service_provider.GetService<InputService>();
    ai_service_ = &service_provider.GetService<AIService>();
    transform_ = &GetEntity().GetComponent<Transform>();
    GetEventBus().Subscribe<OnUpdateEvent>(this);

    if (ai_service_)
    {
        path_to_follow_ = ai_service_->GetPath();
        for (auto p : path_to_follow_)
        {
            std::cout<<p << std::endl;
        }
    }
}

void AIController::OnUpdate(const Timestep& delta_time)
{
}

std::string_view AIController::GetName() const
{
    return "AI Controller";
}