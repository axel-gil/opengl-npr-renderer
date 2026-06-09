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

struct GpuMesh
{
    GLuint vao;
    GLsizei count;
    GLuint tex;
};

static GLuint make_vao(const std::vector<GLfloat>& buf)
{
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(GLfloat), buf.data(),
                 GL_STATIC_DRAW);
    const GLsizei stride = 11 * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    return vao;
}

static GLuint white_tex()
{
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    unsigned char px[4] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 px);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return t;
}

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
             std::vector<GpuMesh> gpu)
{
    // Clear the color and the depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TEST_OPENGL_ERROR();

    float time = (float)glfwGetTime();

    // GLint time_loc = glGetUniformLocation(p->program_id, "time");
    TEST_OPENGL_ERROR();

    /*
    if (time_loc == -1)
    {
        std::cerr << "Uniform 'time' not found" << std::endl;
    }*/

    // glUniform1f(time_loc, time);
    TEST_OPENGL_ERROR();

    mygl::Matrix4 flame_model_matrix = camera.get_view();

    // Bounce
    /*
    flame_model_matrix *= update(time);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE,
                       flame_model_matrix.get_data().data());
                       */

    glUniform1i(glGetUniformLocation(p->program_id, "tex_diffuse"), 0);

    // Set primitives
    GLint tex_loc = glGetUniformLocation(p->program_id, "tex_diffuse");
    for (const auto& g : gpu)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g.tex);
        glUniform1i(tex_loc, 0);
        glBindVertexArray(g.vao);
        glDrawArrays(GL_TRIANGLES, 0, g.count);
    }
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
    (void)argc;
    (void)argv;
    // if (argc != 2)
    // {
    //     std::cerr << "Wrong usage: ./main <file.obj>\n";
    //     return 2;
    // }

    GLFWwindow* window;
    if (!init_GLFW(&window))
        return 2;
    if (!init_glew())
        return 4;
    init_GL();

    mygl::program* p = nullptr;
    if (!init_shaders(&p))
        return 1;
    if (!init_POV(p))
        return 1;

    std::vector<Mesh> meshes;
    if (!load_obj(argv[1], meshes))
    {
        std::cerr << "Wrong obj file\n";
        return 3;
    }

    GLuint white = white_tex();
    std::map<std::string, GLuint> tex_cache;
    std::vector<GpuMesh> gpu;
    for (auto& m : meshes)
    {
        GLuint tex = white;
        if (!m.texture_path.empty())
        {
            auto it = tex_cache.find(m.texture_path);
            if (it != tex_cache.end())
                tex = it->second;
            else
            {
                GLuint t = load_texture(m.texture_path.c_str());
                tex = t ? t : white;
                tex_cache[m.texture_path] = tex;
            }
        }
        gpu.push_back(
            { make_vao(m.buffer), (GLsizei)(m.buffer.size() / 11), tex });
    }
    std::vector<Object> objects = {};

    static const std::vector<GLfloat> g_vertex_buffer_data = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f,  0.0f, 0.5f, 0.5f,  0.0f,
    };

    static const size_t max_particles = 10;
    Billboard billboard{ g_vertex_buffer_data, max_particles };
    billboard.init_particles();

    Camera camera(window);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TEST_OPENGL_ERROR();
        camera.update_camera(p);
        display(p, window, camera, gpu);
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
