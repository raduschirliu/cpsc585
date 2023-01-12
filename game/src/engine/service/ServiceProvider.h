#pragma once

#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "engine/service/Service.h"

class ServiceProvider
{
  public:
    void AddService(std::unique_ptr<IService> service);

    template <class ServiceType>
        requires std::derived_from<ServiceType, IService>
    void AddService(std::unique_ptr<ServiceType> service)
    {
        std::type_index key = std::type_index(typeid(ServiceType));
        services_[key] = std::move(service);
    }

    template <class ServiceType>
        requires std::derived_from<ServiceType, IService>
    ServiceType& GetService()
    {
        auto iterator = services_.find(std::type_index(typeid(ServiceType)));
        ASSERT_MSG(iterator != services_.end(),
                   "Service must exist in the ServiceProvider");

        auto& entry_pair = *iterator;
        return static_cast<ServiceType&>(*entry_pair.second);
    }

  private:
    std::unordered_map<std::type_index, std::unique_ptr<IService>> services_;
};
