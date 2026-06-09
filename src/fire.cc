#include "fire.hh"

void Fire::spawn()
{
    // r = rand [0, 1]
    // angle = [0, 2pi]
    float radius = 4.0f;

    float r = radius * (fast_rand() % 2000) / 20000.0f;
    float angle = 2.0f * M_PI * (fast_rand() % 2000) / 2000;

    float x = std::cos(angle) * r;
    float z = std::sin(angle) * r;

    pos_ = { x, 0, z };

    vel_ = {
        0.2f * (fast_rand() % 2000 - 1000.0f) / 10000.0f,
        1.0f + (fast_rand() % 2000 - 1000.0f) / 10000.0f,
        0.2f * (fast_rand() % 2000 - 1000.0f) / 10000.0f,
    };

    life_ = 1.0f + (fast_rand() % 2000) / 2000.0f;
    max_life_ = life_;
    size_ = 5.0f * (fast_rand() % 2000) / 20000.0f;
    color_ = { 255, 80, 0, 180 };
    alive_ = true;
}

void Fire::update(float dt)
{
    if (life_ <= 0 || size_ <= 0)
    {
        alive_ = false;
        return;
    }

    life_ -= dt;
    pos_ += vel_ * dt;
    vel_ *= { 0.98, 1, 0.98 };
    size_ *= 0.990;

    float t = life_ / max_life_;

    color_.g = 80 + 120 * t;
    // std::cout << static_cast<int>(color_.g) << ", " << t << '\n';

    // std::cout << static_cast<int>(color_.b) << '\n';
    // std::cout << color_.r << ", " << color_.g << ", " << color_.b << '\n';

    // std::cout << "life: " << life_ << ", pos: " << pos_ << "\n";
}
