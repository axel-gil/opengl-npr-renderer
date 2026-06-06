#include "matrix4.hh"
#include <iomanip>
#include <vector>

namespace mygl
{
    Matrix4::Matrix4()
    {}

    Matrix4::Matrix4(std::vector<GLfloat> data)
        : data_(data)
    {}

    Matrix4 Matrix4::identity()
    {
        Matrix4 m = Matrix4();
        m.data_[0] = 1.0f;
        m.data_[5] = 1.0f;
        m.data_[10] = 1.0f;
        m.data_[15] = 1.0f;
        return m;
    }

    void Matrix4::operator*=(const Matrix4& rhs)
    {
        std::vector<GLfloat> result = std::vector<GLfloat>(16, 0);

        for (int col = 0; col < 4; col++)
            for (int row = 0; row < 4; row++)
                for (int k = 0; k < 4; k++)
                    result[col * 4 + row] +=
                        data_[k * 4 + row] * rhs.data_[col * 4 + k];
        data_ = result;
    }

    std::vector<GLfloat> Matrix4::get_data() const
    {
        return data_;
    }

    std::ostream& operator<<(std::ostream& out, const mygl::Matrix4& m)
    {
        for (int row = 0; row < 4; row++)
        {
            out << "[ ";
            for (int col = 0; col < 4; col++)
                out << std::setw(10) << std::fixed << std::setprecision(4)
                    << m.get_data()[col * 4 + row] << " ";
            out << "]\n";
        }
        return out;
    }

} // namespace mygl
