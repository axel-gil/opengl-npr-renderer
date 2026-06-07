#pragma once

namespace mygl
{
    class Vector3
    {
    public:
        Vector3(double x, double y, double z);
        Vector3(const Vector3& Vector3);

        double get_x() const;
        double get_y() const;
        double get_z() const;

        void normalize();
        Vector3 cross(const Vector3& vec) const;
        double dot(const Vector3& vec) const;

        Vector3 operator+(const Vector3& vec) const;
        Vector3 operator-(const Vector3& vec) const;
        Vector3 operator*(const double l) const;
        Vector3 operator/(const double l) const;

    protected:
        double x_;
        double y_;
        double z_;
    };

} // namespace mygl
