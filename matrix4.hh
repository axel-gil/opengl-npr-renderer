#pragma once
#include <ostream>
#include <GL/gl.h>
#include <vector>

namespace mygl
{
    class Matrix4
    {
    public:
        Matrix4();
        Matrix4(std::vector<GLfloat> data);
        void operator*=(const Matrix4& rhs);
        static Matrix4 identity();
        std::vector<GLfloat> get_data() const;

    private:
        std::vector<GLfloat> data_ = std::vector<GLfloat>(16, 0);
    };

    std::ostream& operator<<(std::ostream& out, const mygl::Matrix4& m);
} // namespace mygl
