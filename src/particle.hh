#pragma once
#include "vector3.hh"
#include "color.hh"

class Particle
{
public:
    Particle();
    Particle(mygl::Vector3 pos, mygl::Vector3 vel, const Color& color,
             float life, float size);
    virtual ~Particle() = default;

    Particle& operator=(Particle&& other) = default;

    virtual void spawn();
    virtual void update(float dt);

    const mygl::Vector3& get_pos() const;
    const Color& get_color() const;
    float get_size() const;
    bool is_alive() const;

protected:
    uint32_t fast_rand();

protected:
    mygl::Vector3 pos_;
    mygl::Vector3 vel_;
    Color color_;
    float life_;
    float max_life_;
    float size_;
    bool alive_;
};
