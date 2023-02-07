#pragma once

class Timestep
{
  public:
    static Timestep Milliseconds(double milliseconds);
    static Timestep Seconds(double seconds);

    Timestep();

    void SetMilliseconds(double milliseconds);
    void SetSeconds(double seconds);

    const double GetSeconds() const;
    const double& GetMillis() const;

  private:
    double milliseconds_;
};

/* Binary operators */

Timestep operator+(const Timestep& lhs, const Timestep& rhs);
Timestep operator-(const Timestep& lhs, const Timestep& rhs);
