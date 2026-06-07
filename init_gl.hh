#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "program.hh"
#include "tiny_obj_loader.hh"

bool init_glew();
void init_GL();
bool init_shaders(mygl::program** p);
bool init_object(const std::vector<GLfloat>& obj_buffer, GLuint* vao_id);
bool init_POV(const mygl::program* p);
bool init_GLFW(GLFWwindow** window);
