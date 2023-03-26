#pragma once

#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/gfx/Window.h"
#include "engine/service/Service.h"

class App;

struct ServiceEntry
{
    std::type_index type;
    std::unique_ptr<Service> service;
};

class ServiceProvider
{
  public:
    ServiceProvider() = default;

    template <class ServiceType>
        requires std::derived_from<ServiceType, Service>
    void AddService(std::unique_ptr<ServiceType> service)
    {
        std::type_index key = std::type_index(typeid(ServiceType));

        for (auto& entry : services_)
        {
            ASSERT_MSG(entry.type != key, "Cannot have the same service twice");
        }

        services_.push_back(
            ServiceEntry{.type = key, .service = std::move(service)});
    }

    template <class ServiceType>
        requires std::derived_from<ServiceType, Service>
    ServiceType& GetService() const
    {
        std::type_index key = std::type_index(typeid(ServiceType));

        for (auto& entry : services_)
        {
            if (entry.type == key)
            {
                return static_cast<ServiceType&>(*entry.service);
            }
        }

        // This should never happen at runtime, so throw error and crash
        throw new std::exception("Service does not exist");
    }

    void DispatchInit(App& app);
    void DispatchStart();
    void DispatchSceneLoaded(Scene& scene);
    void DispatchSceneUnloaded(Scene& scene);
    void DispatchUpdate();
    void DispatchWindowSizeChanged(int width, int height);
    void DispatchCleanup();

  private:
    std::vector<ServiceEntry> services_;
};
