#pragma once

namespace math
{

// Map x from [a, b] to y in [c, d]
template <class T>
constexpr T Map(T x, T a, T b, T c, T d)
{
    return (x - a) / (b - a) * (d - c) + c;
}

}  // namespace utils
