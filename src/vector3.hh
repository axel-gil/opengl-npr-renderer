#pragma once

#include <ostream>
#include <vector>
namespace mygl
{
    class Vector3
    {
    public:
        Vector3() = default;
        Vector3(float x, float y, float z);
        Vector3(const Vector3& Vector3);
        Vector3& operator=(Vector3&& Vector3) = default;

        float get_x() const;
        float get_y() const;
        float get_z() const;

        void normalize();
        Vector3 cross(const Vector3& vec) const;
        float dot(const Vector3& vec) const;
        std::vector<float> get_data() const;

        Vector3 operator+(const Vector3& vec) const;
        Vector3 operator-(const Vector3& vec) const;
        Vector3 operator*(const float l) const;
        Vector3 operator/(const float l) const;

        Vector3& operator+=(const Vector3& r);
        Vector3& operator-=(const Vector3& r);
        friend std::ostream& operator<<(std::ostream& os, const Vector3& v);

    protected:
        float x_;
        float y_;
        float z_;
    };

    std::ostream& operator<<(std::ostream& os, const Vector3& v);

} // namespace mygl
