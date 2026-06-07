#pragma once

#include "matrix4.hh"
#include "program.hh"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Camera
{
public:
    Camera(GLFWwindow* window);
    Camera(GLFWwindow* window, GLfloat cam_eye[3], GLfloat cam_center[3],
           GLfloat cam_yaw, GLfloat cam_pitch, const GLfloat cam_rot_speed,
           const GLfloat cam_speed, const GLfloat mouse_sensitivity);
    void update_camera(const mygl::program* p);
    mygl::Matrix4 get_view();

private:
    GLFWwindow* window_;
    GLfloat cam_eye_[3];
    GLfloat cam_center_[3];
    const GLfloat cam_speed_;
    GLfloat cam_yaw_;
    GLfloat cam_pitch_;
    const GLfloat cam_rot_speed_;
    float last_mouse_x_;
    float last_mouse_y_;
    bool mouse_init_;
    const float mouse_sensitivity_;
    mygl::Matrix4 view;
};
