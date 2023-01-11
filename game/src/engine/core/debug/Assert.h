#pragma once

#include <cassert>
#include <type_traits>

#define UNUSED(x) (void)(x)

#define ASSERT_MSG(condition, msg) assert((condition) && msg)

#define STATIC_ASSERT_INTERFACE(type)       \
    static_assert(std::is_abstract<type>(), \
                  "Type '" #type "' must be an abstract class / interface!")

#define TODO(default, msg) \
    assert(false && msg);  \
    return default;