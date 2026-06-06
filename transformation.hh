#pragma once
#include "matrix4.hh"

#include <GL/glew.h>

namespace mygl
{
    Matrix4 lookat(const GLfloat& eyeX, const GLfloat& eyeY,
                   const GLfloat& eyeZ, const GLfloat& centerX,
                   const GLfloat& centerY, const GLfloat& centerZ,
                   const GLfloat& upX, const GLfloat& upY, const GLfloat& upZ);

    Matrix4 frustum(const GLfloat& left, const GLfloat& right,
                    const GLfloat& bottom, const GLfloat& top,
                    const GLfloat& znear, const GLfloat& zfar);

    // Return the dot
    GLfloat dot(const GLfloat v[3], const GLfloat u[3]);

    // Edit the dot in the out parameter
    void cross(const GLfloat v[3], const GLfloat u[3], GLfloat out[3]);

    // Normalize the GLfloat vector
    void normalize(GLfloat v[3]);
} // namespace mygl
