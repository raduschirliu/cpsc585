#include "engine/App.h"

#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"

using std::make_unique;
using std::string;
using std::string_view;

App::App()
    : running_(false),
      window_(),
      service_provider_(),
      scene_list_(),
      event_bus_()
{
}

void App::Run()
{
    // Setup phase
    window_.Create(100, 100, "app");
    window_.SetCallbacks(shared_from_this());

    // Init phase
    OnInit();
    service_provider_.DispatchInit(*this);

    // Run phase
    service_provider_.DispatchStart();
    OnStart();
    PerformGameLoop();

    // Cleanup phase
    OnCleanup();
    service_provider_.DispatchCleanup();
}

void App::OnInit()
{
    // To be overridden if needed
}

void App::OnStart()
{
    // To be overridden if needed
}

void App::OnCleanup()
{
    // To be overridden if needed
}

void App::OnSceneLoaded(Scene& scene)
{
    // To be overridden
}

void App::OnSceneUnloaded(Scene& scene)
{
    // To be overridden
}

Scene& App::AddScene(const string& name)
{
    auto scene = make_unique<Scene>(name, service_provider_);
    return scene_list_.AddScene(std::move(scene));
}

ServiceProvider& App::GetServiceProvider()
{
    return service_provider_;
}

void App::SetActiveScene(const string& name)
{
    if (scene_list_.HasActiveScene())
    {
        Scene& old_scene = scene_list_.GetActiveScene();
        Log::info("Unloading scene: {}", old_scene.GetName());

        old_scene.Unload();
        service_provider_.DispatchSceneUnloaded(old_scene);
        OnSceneUnloaded(old_scene);
    }

    Log::info("Loading scene: {}", name);
    scene_list_.SetActiveScene(name);
    Scene& new_scene = scene_list_.GetActiveScene();

    event_bus_.SetDownstream(&new_scene.GetEventBus());
    service_provider_.DispatchSceneLoaded(new_scene);
    OnSceneLoaded(new_scene);

    Log::info("Active scene set to: {}", name);
}

const Timestep& App::GetDeltaTime() const
{
    return delta_time_;
}

SceneList& App::GetSceneList()
{
    return scene_list_;
}

Window& App::GetWindow()
{
    return window_;
}

EventBus& App::GetEventBus()
{
    return event_bus_;
}

void App::PerformGameLoop()
{
    running_ = true;

    while (running_ && !window_.ShouldClose())
    {
        CalculateDeltaTime();
        window_.PollEvents();

        service_provider_.DispatchUpdate();

        window_.SwapBuffers();
    }
}

void App::CalculateDeltaTime()
{
    const Timestep current_frame = Timestep::Seconds(glfwGetTime());
    delta_time_ = current_frame - last_frame_;
    last_frame_ = current_frame;
}

void App::OnKeyEvent(int key, int scancode, int action, int mods)
{
    InputService::OnKeyEvent(key, scancode, action, mods);
}

void App::OnMouseButtonEvent(int button, int action, int mods)
{
    InputService::OnMouseButtonEvent(button, action, mods);
}

void App::OnCursorMove(double x_pos, double y_pos)
{
    InputService::OnCursorMove(x_pos, y_pos);
}

void App::OnScroll(double xoffset, double yoffset)
{
}

void App::OnWindowSizeChanged(int width, int height)
{
}

void App::OnJoystickChangedEvent(int joystick_id, int event)
{
    InputService::OnJoystickChangedEvent(joystick_id, event);
}