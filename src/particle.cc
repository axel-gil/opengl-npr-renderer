#include "particle.hh"

Particle::Particle(mygl::Vector3 pos, mygl::Vector3 vel, const Color& color,
                   float life, float size)
    : pos_(std::move(pos))
    , vel_(std::move(vel))
    , color_(color)
    , life_(life)
    , size_(size)
{}

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

void Particle::update(float dt)
{
    life_ -= dt;
    pos_ += vel_ * dt;
}
