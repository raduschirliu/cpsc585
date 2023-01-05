#pragma once

#include <cassert>

#define UNUSED(x) (void)(x)

#define ASSERT_MSG(condition, msg) assert((condition)&& msg)

#define TODO(default, msg) \
    assert(false && msg);  \
    return default;