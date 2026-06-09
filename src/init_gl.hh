#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "program.hh"

bool init_glew();
void init_GL();
bool init_shaders(mygl::program** p, std::string shader_name = "");
void init_object(const std::vector<GLfloat>& obj_buffer, GLuint* vao_id);
void init_POV(const mygl::program* p);
bool init_GLFW(GLFWwindow** window);
