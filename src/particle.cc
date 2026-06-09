#include "particle.hh"
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
    , size_(size)
{}

void Particle::spawn()
{
    pos_ = { 0, 0, 0 };

    vel_ = {
        (rand() % 2000 - 1000.0f) / 10000.0f,
        (rand() % 2000 - 1000.0f) / 10000.0f,
        (rand() % 2000 - 1000.0f) / 10000.0f,
    };

    vel_ += { 0, 0.5f, 0 };

    color_ = { 255, 255, 255, 200 };
    life_ = 5.0f;
    size_ = 0.05f;
    alive_ = true;
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

void Particle::update(float dt)
{
    if (life_ <= 0)
    {
        alive_ = false;
        return;
    }

    life_ -= dt;
    pos_ += vel_ * dt;
    color_.b -= dt * 30;
    color_.g -= dt * 20;

    // std::cout << static_cast<int>(color_.b) << '\n';
    // std::cout << color_.r << ", " << color_.g << ", " << color_.b << '\n';

    // std::cout << "life: " << life_ << ", pos: " << pos_ << "\n";
}
