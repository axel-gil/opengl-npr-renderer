#pragma once
#include "vector3.hh"
#include "color.hh"

class Particle
{
public:
    Particle();
    Particle(mygl::Vector3 pos, mygl::Vector3 vel, const Color& color,
             float life, float size);

    Particle& operator=(Particle&& other) = default;

    void spawn();
    void update(float dt);

    const mygl::Vector3& get_pos() const;
    const Color& get_color() const;
    float get_size() const;
    bool is_alive() const;

private:
    mygl::Vector3 pos_;
    mygl::Vector3 vel_;
    Color color_;
    float life_;
    float size_;
    bool alive_;
};
