#pragma once
#include "particle.hh"

class Fire : public Particle
{
public:
    // inherits constructor
    using Particle::Particle;
    ~Fire() = default;

    void spawn() override;
    void update(float dt) override;
};
