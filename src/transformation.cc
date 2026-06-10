#include "transformation.hh"
#include <cmath>
#include <vector>
#include "matrix4.hh"

namespace mygl
{
    GLfloat dot(const GLfloat v[3], const GLfloat u[3])
    {
        return v[0] * u[0] + v[1] * u[1] + v[2] * u[2];
    }

    void cross(const GLfloat v[3], const GLfloat u[3], GLfloat out[3])
    {
        out[0] = v[1] * u[2] - v[2] * u[1];
        out[1] = v[2] * u[0] - v[0] * u[2];
        out[2] = v[0] * u[1] - v[1] * u[0];
    }

    void normalize(GLfloat v[3])
    {
        GLfloat len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }

    Matrix4 lookat(const GLfloat& eyeX, const GLfloat& eyeY,
                   const GLfloat& eyeZ, const GLfloat& centerX,
                   const GLfloat& centerY, const GLfloat& centerZ,
                   const GLfloat& upX, const GLfloat& upY, const GLfloat& upZ)
    {
        // Define f and up
        GLfloat f[] = { centerX - eyeX, centerY - eyeY, centerZ - eyeZ };
        GLfloat up[] = { upX, upY, upZ };

        // Normalize them
        normalize(f);
        normalize(up);

        // Define s
        // s = f x up
        GLfloat s[3];
        cross(f, up, s);

        // Duplicate s
        // GLfloat sn[3] = { s[0], s[1], s[2] };
        // Normalize sn
        normalize(s);

        // Define n
        // u = sn f
        GLfloat u[3];
        cross(s, f, u);

        // Define the plan matrix
        Matrix4 m(std::vector<GLfloat>{ s[0], u[0], -f[0], 0, s[1], u[1], -f[1],
                                        0, s[2], u[2], -f[2], 0, 0, 0, 0, 1 });
        // And the eye matrix
        Matrix4 eye(std::vector<GLfloat>{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
                                          -eyeX, -eyeY, -eyeZ, 1 });
        // Compute them
        m *= eye;

        return m;
    }

    Matrix4 frustum(const GLfloat& left, const GLfloat& right,
                    const GLfloat& bottom, const GLfloat& top,
                    const GLfloat& znear, const GLfloat& zfar)
    {
        // Define matrix values
        GLfloat xnear = 2 * znear / (right - left);
        GLfloat ynear = 2 * znear / (top - bottom);
        GLfloat A = (right + left) / (right - left);
        GLfloat B = (top + bottom) / (top - bottom);
        GLfloat C = -(zfar + znear) / (zfar - znear);
        GLfloat D = -(2 * zfar * znear) / (zfar - znear);

        // Returns them
        return Matrix4(std::vector<GLfloat>{ xnear, 0, 0, 0, 0, ynear, 0, 0, A,
                                             B, C, -1, 0, 0, D, 0 });
    }

    Matrix4 translate(GLfloat x, GLfloat y, GLfloat z)
    {
        return Matrix4(std::vector<GLfloat>{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
                                             x, y, z, 1 });
    }

    Matrix4 scale(GLfloat x, GLfloat y, GLfloat z)
    {
        return Matrix4(std::vector<GLfloat>{ x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0,
                                             0, 0, 0, 1 });
    }
} // namespace mygl
