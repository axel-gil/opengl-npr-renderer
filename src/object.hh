#pragma once
#include "GL/glew.h"
#include "camera.hh"
#include "program.hh"
#include <vector>

class Object
{
public:
    Object(std::vector<GLfloat> buffer, GLuint texture_id, mygl::program* p,
           Camera* c);

    bool init();
    void bounce(float) const;
    void display() const;

private:
    std::vector<GLfloat> buffer;
    GLuint vao_id;
    GLuint texture_id;
    mygl::program* program;
    Camera* camera;
};
