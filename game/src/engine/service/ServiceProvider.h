#pragma once

#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "engine/core/debug/Assert.h"
#include "engine/service/Service.h"

class ServiceProvider
{
  public:
    ServiceProvider() = default;

    template <class ServiceType>
        requires std::derived_from<ServiceType, IService>
    void AddService(std::unique_ptr<ServiceType> service)
    {
        std::type_index key = std::type_index(typeid(ServiceType));
        ASSERT_MSG(services_.find(key) == services_.end(),
                   "Cannot add the same service twice");

        services_[key] = std::move(service);
    }

    template <class ServiceType>
        requires std::derived_from<ServiceType, IService>
    ServiceType& GetService()
    {
        auto iterator = services_.find(std::type_index(typeid(ServiceType)));
        ASSERT_MSG(iterator != services_.end(), "Service must exist");

        auto& entry_pair = *iterator;
        return static_cast<ServiceType&>(*entry_pair.second);
    }

    void OnUpdate();
    void OnCleanup();

  private:
    std::unordered_map<std::type_index, std::unique_ptr<IService>> services_;
};
