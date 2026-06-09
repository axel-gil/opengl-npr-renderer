#include "particle.hh"
#include <cmath>
#include <cstdlib>
#include <iostream>

Particle::Particle()
    : pos_(0, 0, 0)
    , vel_(0, 0, 0)
    , color_(0, 0, 0, 0)
    , life_(0)
    , size_(0)
    , alive_(false)
{}

Particle::Particle(mygl::Vector3 pos, mygl::Vector3 vel, const Color& color,
                   float life, float size)
    : pos_(std::move(pos))
    , vel_(std::move(vel))
    , color_(color)
    , life_(life)
    , max_life_(life)
    , size_(size)
{}

uint32_t Particle::fast_rand()
{
    // seed
    static uint32_t rng_state_ = 42;

    rng_state_ ^= rng_state_ << 13;
    rng_state_ ^= rng_state_ >> 17;
    rng_state_ ^= rng_state_ << 5;
    return rng_state_;
}

void Particle::spawn()
{}

void Particle::update(float dt)
{
    (void)dt;
}

const mygl::Vector3& Particle::get_pos() const
{
    return pos_;
}

const Color& Particle::get_color() const
{
    return color_;
}

float Particle::get_size() const
{
    return size_;
}

bool Particle::is_alive() const
{
    return alive_;
}
