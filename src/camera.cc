#include "camera.hh"

#include <algorithm>
#include <cmath>
#include "matrix4.hh"
#include "transformation.hh"

Camera::Camera(GLFWwindow* window)
    : window_(window)
    , cam_eye_{ 0.0f, 18.f, -40.0f }
    , cam_center_{ 1.0f, 1.5f, 0.0f }
    , cam_yaw_(1.5708f)
    , cam_pitch_(0.0f)
    , cam_speed_(25.0f)
    , cam_rot_speed_(0.04f)
    , last_mouse_x_(0.0f)
    , last_mouse_y_(0.0f)
    , mouse_init_(true)
    , mouse_sensitivity_(0.0025f)
{}

Camera::Camera(GLFWwindow* window, GLfloat cam_eye[3], GLfloat cam_center[3],
               GLfloat cam_yaw, GLfloat cam_pitch, const GLfloat cam_rot_speed,
               const GLfloat cam_speed, const GLfloat mouse_sensitivity)
    : window_(window)
    , cam_yaw_(cam_yaw)
    , cam_pitch_(cam_pitch)
    , cam_speed_(cam_speed)
    , cam_rot_speed_(cam_rot_speed)
    , last_mouse_x_(0.0f)
    , last_mouse_y_(0.0f)
    , mouse_init_(true)
    , mouse_sensitivity_(mouse_sensitivity)
{
    std::copy(cam_eye, cam_eye + 3, cam_eye_);
    std::copy(cam_center, cam_center + 3, cam_center_);
}

void Camera::update_camera(const mygl::program* p)
{
    // get the current mouse position
    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(window_, &mouse_x, &mouse_y);

    // avoid camera bug on opengl start up
    if (mouse_init_)
    {
        last_mouse_x_ = mouse_x;
        last_mouse_y_ = mouse_y;
        mouse_init_ = false;
    }

    // get the cam delta
    float delta_x = mouse_x - last_mouse_x_;
    float delta_y = mouse_y - last_mouse_y_;
    last_mouse_x_ = mouse_x;
    last_mouse_y_ = mouse_y;

    // update rotation constants
    // - horizontal axis
    cam_yaw_ += delta_x * mouse_sensitivity_;

    // - vertical axis
    cam_pitch_ -= delta_y * mouse_sensitivity_;

    const float lim = 1.55f;
    if (cam_pitch_ > lim)
        cam_pitch_ = lim;
    if (cam_pitch_ < -lim)
        cam_pitch_ = -lim;

    // forward axis vector calculated from the new angles
    float forward_x = std::cos(cam_pitch_) * std::cos(cam_yaw_);
    float forward_y = std::sin(cam_pitch_);
    float forward_z = std::cos(cam_pitch_) * std::sin(cam_yaw_);

    // forward but we dont take pitch as a param to move only on the horizontal
    // axis
    float move_fx = std::cos(cam_yaw_);
    float move_fz = std::sin(cam_yaw_);

    // right axis vector normalized
    float right_x = -move_fz;
    float right_z = move_fx;
    float rlen = std::sqrt(right_x * right_x + right_z * right_z);
    right_x /= rlen;
    right_z /= rlen;

    // translations movements
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam_eye_[0] += move_fx * cam_speed_;
        cam_eye_[2] += move_fz * cam_speed_;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam_eye_[0] -= move_fx * cam_speed_;
        cam_eye_[2] -= move_fz * cam_speed_;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam_eye_[0] += right_x * cam_speed_;
        cam_eye_[2] += right_z * cam_speed_;
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam_eye_[0] -= right_x * cam_speed_;
        cam_eye_[2] -= right_z * cam_speed_;
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cam_eye_[1] += cam_speed_;
    }
    if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cam_eye_[1] -= cam_speed_;
    }
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }

    cam_center_[0] = cam_eye_[0] + forward_x;
    cam_center_[1] = cam_eye_[1] + forward_y;
    cam_center_[2] = cam_eye_[2] + forward_z;

    view = mygl::lookat(cam_eye_[0], cam_eye_[1], cam_eye_[2], cam_center_[0],
                        cam_center_[1], cam_center_[2], 0.0f, 1.0f, 0.0f);

    GLint view_loc = glGetUniformLocation(p->program_id, "model_view");

    if (view_loc != -1)
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.get_data().data());
}

mygl::Matrix4 Camera::get_view()
{
    return view;
}
