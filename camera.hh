#pragma once

#include "program.hh"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern GLFWwindow* window;
extern GLfloat cam_eye[3];
extern GLfloat cam_center[3];
extern const GLfloat cam_speed;
extern GLfloat cam_yaw;
extern GLfloat cam_pitch;
extern const GLfloat cam_rot_speed;
extern float last_mouse_x;
extern float last_mouse_y;
extern bool first_mouse;
extern const float mouse_sensitivity;

void update_camera(const mygl::program* p);
