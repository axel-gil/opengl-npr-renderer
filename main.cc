#include <cmath>
#include "camera.hh"
#ifdef __APPLE__
#    define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

#include "matrix4.hh"
#include "transformation.hh"
// #include "object_vbo.hh"
#include "program.hh"

#include "tiny_obj_loader.hh"

#define TEST_OPENGL_ERROR()                                                    \
    do                                                                         \
    {                                                                          \
        GLenum err = glGetError();                                             \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl;             \
        }                                                                      \
    } while (0)

// Globals
GLuint vao_id;

// std::vector<GLfloat> vertex_buffer_data;
// std::vector<GLfloat> normal_buffer_data;
// std::vector<GLfloat> texture_buffer_data;
// std::vector<GLfloat> color_buffer_data;

GLfloat cam_eye[3] = { 0.0f, 1.5f, -4.0f };
GLfloat cam_center[3] = { 1.0f, 1.5f, 0.0f };

// Glut window_resize function
void window_resize(int width, int height)
{
    // std::cout << "glViewport(0,0,"<< width << "," << height <<
    // ");TEST_OPENGL_ERROR();" << std::endl;
    glViewport(0, 0, width, height);
    TEST_OPENGL_ERROR();
}

// Init OpenGLfunctions
bool init_glew()
{
    // Try to add all OpenGL functions pointers at runtime
    // Return an error if failing
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error while initializing glew" << std::endl;
        std::cerr << glewGetErrorString(err) << std::endl;
        return false;
    }
    glGetError();
    return true;
}

// Init OpenGL
void init_GL()
{
    // Make OpenGL vigilant on object overwrite with the z buffer
    glEnable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();

    // Backface culling
    glEnable(GL_CULL_FACE);
    TEST_OPENGL_ERROR();

    // Added polygons are filled
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    TEST_OPENGL_ERROR();

    // Background color used by clear
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    TEST_OPENGL_ERROR();
}

// Init the shaders
bool init_shaders(mygl::program** p)
{
    mygl::program* program =
        mygl::program::makeprogram("vertex.shd", "fragment.shd");
    *p = program;

    // Use the program in the rendering state
    if (program->is_ready())
    {
        program->use();
        TEST_OPENGL_ERROR();
    }
    else
    {
        std::cerr << program->get_log();
        return false;
    }

    return true;
}

// Init the global
bool init_object(std::vector<GLfloat> obj_buffer)
{
    // Generate 1 vertex array
    // The name is stored in vao_id (global variable)
    GLuint vbo_ids[1];
    glGenVertexArrays(1, &vao_id);
    TEST_OPENGL_ERROR(); // Start recording the VAO
    // Bindings below will be remembered
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // Generate 1 VBO containing 4 attributes.
    // it is shaped like this:
    // xyz nxnynz uv rgb
    // position, normal, texture, color
    glGenBuffers(1, vbo_ids);
    TEST_OPENGL_ERROR();

    // Bind the first VBO as the active one for GL_ARRAY_BUFFER operations
    // glBufferData & glVertexAttribPointer are calling VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[0]);
    TEST_OPENGL_ERROR();

    // Upload the CPU vector in GPU memory
    // GL_STATIC_DRAW hints the GPU that you intend to draw multiples times and
    // he will choose the best way to optimize your code using that

    // The buffer is organized like this:
    // xyz nxnynz uv rgb

    glBufferData(GL_ARRAY_BUFFER, obj_buffer.size() * sizeof(GLfloat),
                 obj_buffer.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    // xyz
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          (void*)0);
    glEnableVertexAttribArray(0);

    // nxnynz
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // rgb
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    return true;
}

bool init_POV(const mygl::program* p)
{
    mygl::Matrix4 view =
        mygl::lookat(0.0f, 1.5f, -4.0f, 1.0f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);

    // FOV of 90
    float fovy = 90.0f * M_PI / 180.0f;
    float aspect = 1920.0 / 1080;
    float znear = 0.1f, zfar = 100.0f;

    float top = znear * std::tan(fovy * 0.5f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;
    mygl::Matrix4 proj = mygl::frustum(left, right, bottom, top, znear, zfar);

    // Get the uniformLocation in the program with a named uniform variable
    // Return -1 if the variable doesn't exist
    GLint view_loc = glGetUniformLocation(p->program_id, "model_view");
    TEST_OPENGL_ERROR();

    GLint proj_loc = glGetUniformLocation(p->program_id, "projection");
    TEST_OPENGL_ERROR();

    if (view_loc == -1)
        std::cerr << "Uniform 'model_view' not found" << std::endl;
    if (proj_loc == -1)
        std::cerr << "Uniform 'projection' not found" << std::endl;

    // Upload the two 4x4 matrices on the GPU
    // count : 1
    // transpose : GL_FALSE
    // value: the matrix
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.get_data().data());
    TEST_OPENGL_ERROR();
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, proj.get_data().data());
    TEST_OPENGL_ERROR();

    // Get the light direction
    GLint light_loc = glGetUniformLocation(p->program_id, "light_dir");
    TEST_OPENGL_ERROR();
    if (light_loc == -1)
    {
        std::cerr << "Uniform 'light_dir' not found" << std::endl;
    }

    // Set light position
    glUniform3f(light_loc, 1.0f, 1.0f, 1.0f);
    TEST_OPENGL_ERROR();

    return true;
}

bool init_GLFW(GLFWwindow** window)
{
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS

    GLFWwindow* w = glfwCreateWindow(2560, 1440, "TestOpenGL", NULL, NULL);
    if (!w)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(w);

    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(w, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    *window = w;

    return true;
}

void display(const mygl::program* p, GLFWwindow* window, size_t vertex_count)
{
    // Clear the color and the depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TEST_OPENGL_ERROR();

    // Activate the VAO
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // Get the uniformLocation of object_color
    GLint color_loc = glGetUniformLocation(p->program_id, "object_color");
    TEST_OPENGL_ERROR();

    if (color_loc == -1)
    {
        std::cerr << "Uniform 'color_location' not found" << std::endl;
    }

    // // Upload a warm color fot the object
    // // This variable is used for the object as a fiffuse color in the
    // // lighting
    glUniform3f(color_loc, 0.95f, 0.4f, 0.f);
    TEST_OPENGL_ERROR();

    GLint time_loc = glGetUniformLocation(p->program_id, "time");
    TEST_OPENGL_ERROR();

    if (color_loc == -1)
    {
        std::cerr << "Uniform 'time' not found" << std::endl;
    }

    // // Upload a warm color fot the object
    // // This variable is used for the object as a fiffuse color in the
    // // lighting
    glUniform1f(time_loc, (float)glfwGetTime());
    TEST_OPENGL_ERROR();

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

bool load_obj(const std::string& path, std::vector<GLfloat>& result_buffer)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./"; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config))
    {
        if (!reader.Error().empty())
            std::cerr << "TinyObjReader error: " << reader.Error();
        return false;
    }

    if (!reader.Warning().empty())
        std::cerr << "TinyObjReader warning: " << reader.Warning();

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f]; // 3 or 4 (or more)

            // Fan triangulation: triangle = (0, v, v+1) for v in [1, fv-2]
            for (int v = 1; v <= fv - 2; v++)
            {
                for (int corner : { 0, v, v + 1 })
                {
                    tinyobj::index_t idx =
                        shape.mesh.indices[index_offset + corner];

                    // Position
                    result_buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 0]);
                    result_buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 1]);
                    result_buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 2]);

                    if (idx.normal_index >= 0)
                    {
                        result_buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 0]);
                        result_buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 1]);
                        result_buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 2]);
                    }
                    else
                        return false;

                    if (idx.texcoord_index >= 0)
                    {
                        result_buffer.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 0]);
                        result_buffer.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 1]);
                    }
                    else
                        return false;

                    result_buffer.push_back(
                        attrib.colors[3 * idx.vertex_index + 0]);
                    result_buffer.push_back(
                        attrib.colors[3 * idx.vertex_index + 1]);
                    result_buffer.push_back(
                        attrib.colors[3 * idx.vertex_index + 2]);
                }
            }
            index_offset += fv;
        }
    }

    return true;
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
    if (!init_object(obj_buffer))
        return 1;
    if (!init_POV(p))
        return 1;

    Camera camera = Camera(window);

    while (!glfwWindowShouldClose(window))
    {
        camera.update_camera(p);

        display(p, window, obj_buffer.size() / 11);
    }

    delete p;
    glfwTerminate();

    return 0;
}
