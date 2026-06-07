#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

#include "gl_error.hh"
#include "matrix4.hh"
#include "transformation.hh"
#include "program.hh"
#include "camera.hh"

#include "tiny_obj_loader.hh"

#include "init_gl.hh"

// Globals
GLuint vao_id;

static mygl::Matrix4 update(float time)
{
    float flicker_y = 1 + 0.15f * sin(time * 15.0f);
    float flicker_x = 1 + 0.05f * sin(time * 17.0f);

    mygl::Matrix4 t = mygl::translate(0.0f, 0.5f, 0.0f);
    mygl::Matrix4 s = mygl::scale(flicker_x, flicker_y, flicker_x);
    t *= s;

    return t;
}

void display(const mygl::program* p, GLFWwindow* window, Camera& camera,
             size_t vertex_count)
{
    // Clear the color and the depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TEST_OPENGL_ERROR();

    // Activate the VAO
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    GLint time_loc = glGetUniformLocation(p->program_id, "time");
    TEST_OPENGL_ERROR();
    /*
        if (time_loc == -1)
        {
            std::cerr << "Uniform 'time' not found" << std::endl;
        }*/

    // Upload a warm color fot the object
    // This variable is used for the object as a fiffuse color in the
    // lighting
    float time = (float)glfwGetTime();
    glUniform1f(time_loc, time);
    TEST_OPENGL_ERROR();
    GLint model_loc = glGetUniformLocation(p->program_id, "model_view");

    mygl::Matrix4 flame_model_matrix = camera.get_view();
    flame_model_matrix *= update(time);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE,
                       flame_model_matrix.get_data().data());

    // Set primitives
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    TEST_OPENGL_ERROR();

    // Unbind the VAO
    glBindVertexArray(0);
    TEST_OPENGL_ERROR();

    // Swaps the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Wrong usage: ./main <file.obj>\n";
        return 2;
    }

    // xyz nxnynz uv rgb
    std::vector<GLfloat> obj_buffer;

    if (!load_obj(argv[1], obj_buffer))
    {
        std::cerr << "Wrong obj file\n";
        return 3;
    }

    // Create GL window and context
    GLFWwindow* window;

    if (!init_GLFW(&window))
        return 2;

    // Loads GL functions pointers
    if (!init_glew())
        return 4;

    // Set global rendering state
    init_GL();

    mygl::program* p = nullptr;

    if (!init_shaders(&p))
        return 1;
    if (!init_object(obj_buffer, &vao_id))
        return 1;
    if (!init_POV(p))
        return 1;

    Camera camera = Camera(window);

    while (!glfwWindowShouldClose(window))
    {
        camera.update_camera(p);

        display(p, window, camera, obj_buffer.size() / 11);
    }

    delete p;
    glfwTerminate();

    return 0;
}
