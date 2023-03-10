#pragma once

#include "engine/gui/OnGuiEvent.h"
#include "engine/input/InputService.h"
#include "engine/service/Service.h"

class SceneDebugService final : public Service,
                                public IEventSubscriber<OnGuiEvent>
{
  public:
    // From Service
    void OnInit() override;
    void OnStart(ServiceProvider& service_provider) override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    void SetActiveScene(const std::string& name);

    // From IEventSubscriber<OnGuiEvent>
    void OnGui() override;

  private:
    jss::object_ptr<InputService> input_service_;
    jss::object_ptr<Scene> active_scene_;
    bool show_demo_menu_ = false;
    bool show_menu_ = false;
    int framerate_;
    double prev_time_;
    int frame_count_;

    void DrawGeneralTab();
    void DrawEntityTab();
    void DrawSceneTab();
};
