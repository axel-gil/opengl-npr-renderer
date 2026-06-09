#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "fire.hh"
#include "gl_error.hh"
#include "program.hh"
#include "camera.hh"

#include "tiny_obj_loader.hh"

#include "init_gl.hh"
#include "object.hh"
#include "billboard.hh"
#include "utils.hh"

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

void display(const Program* p, const Program* outline, GLFWwindow* window,
             Camera& camera, const std::vector<GpuMesh>& gpu)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mygl::Matrix4 view = camera.get_view();

    outline->use();
    glUniformMatrix4fv(
        glGetUniformLocation(outline->get_program_id(), "model_view"), 1,
        GL_FALSE, view.get_data().data());
    glUniform1f(
        glGetUniformLocation(outline->get_program_id(), "outline_width"),
        0.05f);
    glCullFace(GL_FRONT);
    for (const auto& g : gpu)
    {
        glBindVertexArray(g.vao);
        glDrawArrays(GL_TRIANGLES, 0, g.count);
    }

    p->use();
    glCullFace(GL_BACK);
    GLint tex_loc = glGetUniformLocation(p->get_program_id(), "tex_diffuse");
    glActiveTexture(GL_TEXTURE0);
    for (const auto& g : gpu)
    {
        glBindTexture(GL_TEXTURE_2D, g.tex);
        glUniform1i(tex_loc, 0);
        glBindVertexArray(g.vao);
        glDrawArrays(GL_TRIANGLES, 0, g.count);
    }

    glBindVertexArray(0);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Wrong usage: ./main <file.obj>\n";
        return 2;
    }

    GLFWwindow* window;
    if (!init_GLFW(&window))
        return 2;
    if (!init_glew())
        return 4;
    init_GL();

    std::shared_ptr<Program> fire = std::make_shared<Program>();
    if (!fire->init_shaders("fire"))
    {
        return 1;
    }
    fire->init_POV();

    std::shared_ptr<Program> color = std::make_shared<Program>();
    if (!color->init_shaders("color"))
    {
        return 1;
    }
    color->init_POV();

    std::shared_ptr<Program> outline = std::make_shared<Program>();
    if (!outline->init_shaders("outline"))
    {
        return 1;
    }
    outline->init_POV();

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
    static const size_t max_particles = 1000000;

    Billboard<Fire> billboard{ max_particles };
    billboard.init_particles();

    Camera camera(window);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TEST_OPENGL_ERROR();
        camera.update_camera(fire.get());
        camera.update_camera(color.get());
        camera.update_camera(outline.get());

        fire->use();
        billboard.update_particles();
        billboard.display();

        display(color.get(), outline.get(), window, camera, gpu);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
