#include "point3.hh"

#include <cmath>

#include "vector3.hh"

Point3::Point3(double x, double y, double z)
    : x_(x)
    , y_(y)
    , z_(z)
{}

Point3::Point3(const Point3& point3)
    : x_(point3.get_x())
    , y_(point3.get_y())
    , z_(point3.get_z())
{}

double Point3::get_x() const
{
    return x_;
}
double Point3::get_y() const
{
    return y_;
}
double Point3::get_z() const
{
    return z_;
}

void Point3::normalize()
{
    double euclidian_lenght = sqrt(x_ * x_ + y_ * y_ + z_ * z_);
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

Vector3 Point3::cross(const Point3& p) const
{
    return Vector3{ y_ * p.get_z() - z_ * p.get_y(),
                    z_ * p.get_x() - x_ * p.get_z(),
                    x_ * p.get_y() - y_ * p.get_x() };
}

double Point3::dot(const Point3& p) const
{
    return x_ * p.get_x() + y_ * p.get_y() + z_ * p.get_z();
}

Vector3 Point3::operator+(const Point3& p) const
{
    return Vector3{ p.get_x() + get_x(), p.get_y() + get_y(),
                    p.get_z() + get_z() };
}

Vector3 Point3::operator-(const Point3& p) const
{
    return Vector3{ get_x() - p.get_x(), get_y() - p.get_y(),
                    get_z() - p.get_z() };
}

Vector3 Point3::operator*(const double l) const
{
    return Vector3{ get_x() * l, get_y() * l, get_z() * l };
}

Vector3 Point3::operator/(const double l) const
{
    return Vector3{ get_x() / l, get_y() / l, get_z() / l };
}
