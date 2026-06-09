#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

void display(const mygl::program* p, const mygl::program* outline,
             GLFWwindow* window, Camera& camera,
             const std::vector<GpuMesh>& gpu)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mygl::Matrix4 view = camera.get_view();

    // ---- Passe 1 : coque de contour, faces AVANT cullées ----
    outline->use();
    glUniformMatrix4fv(glGetUniformLocation(outline->program_id, "model_view"),
                       1, GL_FALSE, view.get_data().data());
    glUniform1f(glGetUniformLocation(outline->program_id, "outline_width"),
                0.05f);
    glCullFace(GL_FRONT);
    for (const auto& g : gpu)
    {
        glBindVertexArray(g.vao);
        glDrawArrays(GL_TRIANGLES, 0, g.count);
    }

    // ---- Passe 2 : objet normal, faces ARRIÈRE cullées ----
    p->use();
    glCullFace(GL_BACK);
    GLint tex_loc = glGetUniformLocation(p->program_id, "tex_diffuse");
    glActiveTexture(GL_TEXTURE0);
    for (const auto& g : gpu)
    {
        glBindTexture(GL_TEXTURE_2D, g.tex);
        glUniform1i(tex_loc, 0);
        glBindVertexArray(g.vao);
        glDrawArrays(GL_TRIANGLES, 0, g.count);
    }

    glBindVertexArray(0);
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

    GLFWwindow* window;
    if (!init_GLFW(&window))
        return 2;
    if (!init_glew())
        return 4;
    init_GL();

    mygl::program* p = nullptr;
    if (!init_shaders(&p))
        return 1;
    init_POV(p);

    mygl::program* outline = mygl::program::makeprogram("outline_vertex.shd",
                                                        "outline_fragment.shd");
    if (!outline->is_ready())
    {
        std::cerr << outline->get_log();
        return 1;
    }
    outline->use();
    init_POV(outline);

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
        camera.update_camera(p);
        // display(p, window, camera, gpu);
        billboard.update_particles();

        billboard.display();
        // display(p, window, objects);

        glfwSwapBuffers(window);
        glfwPollEvents();
        // p->use();
        // camera.update_camera(p);
        // display(p, outline, window, camera, gpu);
    }

    delete p;
    glfwTerminate();
    return 0;
}
