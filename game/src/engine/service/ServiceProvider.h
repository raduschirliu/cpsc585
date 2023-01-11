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
        // TODO(radu): Write this
    }

  private:
    std::unordered_map<std::type_index, std::unique_ptr<IService>> services_;
};
