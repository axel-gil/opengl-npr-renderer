#include "camera.hh"

#include <cmath>
#include "matrix4.hh"
#include "transformation.hh"

void update_camera(const mygl::program* p)
{
    // get the current mouse position
    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    // avoid camera bug on opengl start up
    if (first_mouse)
    {
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
        first_mouse = false;
    }

    // get the cam delta
    float delta_x = mouse_x - last_mouse_x;
    float delta_y = mouse_y - last_mouse_y;
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;

    // update rotation constants
    // - horizontal axis
    cam_yaw += delta_x * mouse_sensitivity;

    // - vertical axis
    cam_pitch -= delta_y * mouse_sensitivity;

    const float lim = 1.55f;
    if (cam_pitch > lim)
        cam_pitch = lim;
    if (cam_pitch < -lim)
        cam_pitch = -lim;

    // forward axis vector calculated from the new angles
    float forward_x = std::cos(cam_pitch) * std::cos(cam_yaw);
    float forward_y = std::sin(cam_pitch);
    float forward_z = std::cos(cam_pitch) * std::sin(cam_yaw);

    // right axis vector normalized
    float right_x = -forward_z;
    float right_z = forward_x;
    float rlen = std::sqrt(right_x * right_x + right_z * right_z);
    right_x /= rlen;
    right_z /= rlen;

    // translations movements
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam_eye[0] += forward_x * cam_speed;
        cam_eye[1] += forward_y * cam_speed;
        cam_eye[2] += forward_z * cam_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam_eye[0] -= forward_x * cam_speed;
        cam_eye[1] -= forward_y * cam_speed;
        cam_eye[2] -= forward_z * cam_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam_eye[0] += right_x * cam_speed;
        cam_eye[2] += right_z * cam_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam_eye[0] -= right_x * cam_speed;
        cam_eye[2] -= right_z * cam_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cam_eye[1] += cam_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cam_eye[1] -= cam_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    cam_center[0] = cam_eye[0] + forward_x;
    cam_center[1] = cam_eye[1] + forward_y;
    cam_center[2] = cam_eye[2] + forward_z;

    mygl::Matrix4 view =
        mygl::lookat(cam_eye[0], cam_eye[1], cam_eye[2], cam_center[0],
                     cam_center[1], cam_center[2], 0.0f, 1.0f, 0.0f);

    GLint view_loc = glGetUniformLocation(p->program_id, "model_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.get_data().data());
}
