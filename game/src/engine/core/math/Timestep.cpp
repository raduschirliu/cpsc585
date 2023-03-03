#include "engine/core/math/Timestep.h"

static constexpr double kSecondsToMillils = 1000.0f;
static constexpr double kMillisToSeconds = 1.0f / 1000.0f;

Timestep Timestep::Milliseconds(double milliseconds)
{
    Timestep timestep;
    timestep.SetMilliseconds(milliseconds);
    return timestep;
}

Timestep Timestep::Seconds(double seconds)
{
    Timestep timestep;
    timestep.SetSeconds(seconds);
    return timestep;
}

Timestep::Timestep() : milliseconds_(0.0f)
{
}

void Timestep::SetMilliseconds(double milliseconds)
{
    milliseconds_ = milliseconds;
}

void Timestep::SetSeconds(double seconds)
{
    milliseconds_ = seconds * kSecondsToMillils;
}

const double Timestep::GetSeconds() const
{
    return milliseconds_ * kMillisToSeconds;
}

const double& Timestep::GetMillis() const
{
    return milliseconds_;
}

/* Binary operators */

Timestep operator+(const Timestep& lhs, const Timestep& rhs)
{
    Timestep result;
    result.SetMilliseconds(lhs.GetMillis() + rhs.GetMillis());
    return result;
}

Timestep operator-(const Timestep& lhs, const Timestep& rhs)
{
    Timestep result;
    result.SetMilliseconds(lhs.GetMillis() - rhs.GetMillis());
    return result;
}

bool Timestep::operator==(const Timestep& rhs)
{
    return milliseconds_ == rhs.milliseconds_;
}

bool Timestep::operator>(const Timestep& rhs)
{
    return milliseconds_ > rhs.milliseconds_;
}

bool Timestep::operator<(const Timestep& rhs)
{
    return milliseconds_ < rhs.milliseconds_;
}

bool Timestep::operator>=(const Timestep& rhs)
{
    return milliseconds_ >= rhs.milliseconds_;
}

bool Timestep::operator<=(const Timestep& rhs)
{
    return milliseconds_ <= rhs.milliseconds_;
}

Timestep& Timestep::operator+=(const Timestep& rhs)
{
    milliseconds_ += rhs.milliseconds_;
    return *this;
}

Timestep& Timestep::operator-=(const Timestep& rhs)
{
    milliseconds_ -= rhs.milliseconds_;
    return *this;
}