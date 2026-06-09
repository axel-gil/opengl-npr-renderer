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
#include "utils.hh"
#include "object.hh"
#include "billboard.hh"

void display(const mygl::program* p, GLFWwindow* window,
             std::vector<Object> objects)
{
    // Clear the color and the depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TEST_OPENGL_ERROR();

    float time = (float)glfwGetTime();

    GLint time_loc = glGetUniformLocation(p->program_id, "time");
    TEST_OPENGL_ERROR();
    /*
        if (time_loc == -1)
        {
            std::cerr << "Uniform 'time' not found" << std::endl;
        }*/

    glUniform1f(time_loc, time);
    TEST_OPENGL_ERROR();

    for (const auto& object : objects)
    {
        // object.bounce(time);
        object.display();
    }

    // Swaps the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    // if (argc != 2)
    // {
    //     std::cerr << "Wrong usage: ./main <file.obj>\n";
    //     return 2;
    // }

    // // xyz nxnynz uv rgb
    // std::vector<GLfloat> obj_buffer;
    // if (!load_obj(argv[1], obj_buffer))
    // {
    //     std::cerr << "Wrong obj file\n";
    //     return 3;
    // }

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

    if (!init_POV(p))
        return 1;

    // GLuint tex = load_texture(
    //     "textures/tripo_mat_0fd31f00-a609-4dff-92d1-6f84a6da7224_diffuse.jpeg");
    Camera camera = Camera(window);

    // Object house{ obj_buffer, tex, p, &camera };

    // if (!house.init())
    //     return 1;
    std::vector<Object> objects = {};

    static const std::vector<GLfloat> g_vertex_buffer_data = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f,  0.0f, 0.5f, 0.5f,  0.0f,
    };

    static const size_t max_particles = 1000000;
    Billboard billboard{ g_vertex_buffer_data, max_particles };
    billboard.init_particles();

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TEST_OPENGL_ERROR();
        camera.update_camera(p);
        billboard.update_particles();

        billboard.display();
        // display(p, window, objects);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete p;
    glfwTerminate();

    return 0;
}
