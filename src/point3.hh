#pragma once

class Vector3;

class Point3
{
public:
    Point3(double x, double y, double z);
    Point3(const Point3& Point3);

    double get_x() const;
    double get_y() const;
    double get_z() const;

    void normalize();
    Vector3 cross(const Point3& vec) const;
    double dot(const Point3& vec) const;

    Vector3 operator+(const Point3& vec) const;
    Vector3 operator-(const Point3& vec) const;
    Vector3 operator*(const double l) const;
    Vector3 operator/(const double l) const;

protected:
    double x_;
    double y_;
    double z_;
};
