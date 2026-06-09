#pragma once

#include <cstdint>
class Color
{
public:
    Color() = default;
    Color(uint8_t r, uint8_t b, uint8_t g, uint8_t a);
    Color(const Color& other) = default;
    Color& operator=(Color& other) = default;
    Color& operator=(Color&& other) = default;

public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};
