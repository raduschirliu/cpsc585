#include "engine/core/math/Random.h"

#include <random>

std::random_device kRandomSeed;
std::mt19937 kRandomGenerator(kRandomSeed());
std::uniform_int_distribution<int> value(0, 100);

namespace math
{

int RandomInt(int low_bound, int high_bound)
{
    std::uniform_int_distribution<int> random(low_bound, high_bound);
    return random(kRandomGenerator);
}

float RandomFloat(float low_bound, float high_bound)
{
    std::uniform_real_distribution<float> random(low_bound, high_bound);
    return random(kRandomGenerator);
}

double RandomDouble(double low_bound, double high_bound)
{
    std::uniform_real_distribution<double> random(low_bound, high_bound);
    return random(kRandomGenerator);
}

}  // namespace math
