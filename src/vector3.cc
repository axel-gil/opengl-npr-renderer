#include "vector3.hh"

#include <cmath>
#include <vector>

namespace mygl
{
    Vector3::Vector3(float x, float y, float z)
        : x_(x)
        , y_(y)
        , z_(z)
    {}

    Vector3::Vector3(const Vector3& point3)
        : x_(point3.get_x())
        , y_(point3.get_y())
        , z_(point3.get_z())
    {}

    float Vector3::get_x() const
    {
        return x_;
    }
    float Vector3::get_y() const
    {
        return y_;
    }
    float Vector3::get_z() const
    {
        return z_;
    }

    void Vector3::normalize()
    {
        float euclidian_lenght = sqrt(x_ * x_ + y_ * y_ + z_ * z_);
        if (euclidian_lenght != 0)
        {
            x_ /= euclidian_lenght;
            y_ /= euclidian_lenght;
            z_ /= euclidian_lenght;
            return;
        }
        x_ = 0;
        y_ = 0;
        z_ = 0;
    }

    Vector3 Vector3::cross(const Vector3& p) const
    {
        return Vector3{ y_ * p.get_z() - z_ * p.get_y(),
                        z_ * p.get_x() - x_ * p.get_z(),
                        x_ * p.get_y() - y_ * p.get_x() };
    }

    float Vector3::dot(const Vector3& p) const
    {
        return x_ * p.get_x() + y_ * p.get_y() + z_ * p.get_z();
    }

    std::vector<float> Vector3::get_data() const
    {
        return std::vector<float>{ x_, y_, z_ };
    }

    Vector3 Vector3::operator+(const Vector3& p) const
    {
        return Vector3{ p.get_x() + get_x(), p.get_y() + get_y(),
                        p.get_z() + get_z() };
    }

    Vector3 Vector3::operator-(const Vector3& p) const
    {
        return Vector3{ get_x() - p.get_x(), get_y() - p.get_y(),
                        get_z() - p.get_z() };
    }

    Vector3 Vector3::operator*(const float l) const
    {
        return Vector3{ get_x() * l, get_y() * l, get_z() * l };
    }

    Vector3 Vector3::operator/(const float l) const
    {
        return Vector3{ get_x() / l, get_y() / l, get_z() / l };
    }

    Vector3& Vector3::operator+=(const Vector3& r)
    {
        x_ += r.x_;
        y_ += r.y_;
        z_ += r.z_;

        return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& r)
    {
        x_ -= r.x_;
        y_ -= r.y_;
        z_ -= r.z_;

        return *this;
    }

} // namespace mygl
