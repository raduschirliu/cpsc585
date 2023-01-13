#pragma once

#include <string_view>

#include "engine/core/debug/Assert.h"

class IService
{
  public:
    virtual void OnInit() = 0;
    virtual void OnStart() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnCleanup() = 0;

    virtual std::string_view GetName() const = 0;
};

STATIC_ASSERT_INTERFACE(IService);