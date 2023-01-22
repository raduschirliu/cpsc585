#include "engine/App.h"

#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"

using std::make_unique;
using std::string_view;

App::App()
    : running_(false),
      window_(),
      scenes_{},
      service_provider_(),
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
    service_provider_.DispatchInit(window_, event_bus_);

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

Scene& App::AddScene(string_view name)
{
    auto scene = make_unique<Scene>(name, service_provider_, event_bus_);
    scenes_.push_back(std::move(scene));

    return *scenes_.back();
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
        window_.PollEvents();

        // TODO: Use fixed timestep for physics?
        // TODO: Send update event to components in the active scene
        service_provider_.DispatchUpdate();

        window_.SwapBuffers();
    }
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
