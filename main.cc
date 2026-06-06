#ifdef __APPLE__
#    define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "matrix4.hh"
#include "transformation.hh"
// #include "object_vbo.hh"
#include "program.hh"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.hh"

// #define SAVE_RENDER
// #if defined(SAVE_RENDER)
//   bool saved = false;
// #endif

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
GLFWwindow* window;

std::vector<GLfloat> vertex_buffer_data;
std::vector<GLfloat> normal_buffer_data;
std::vector<GLfloat> texture_buffer_data;
std::vector<GLfloat> color_buffer_data;

/*
std::vector<GLfloat> vertex_buffer_data = {
    -1, -1, 1,  1,  -1, 1,  0, 1,  0, 1,  -1, 1,  1,  -1, -1, 0, 1,  0,
    1,  -1, -1, -1, -1, -1, 0, 1,  0, -1, -1, -1, -1, -1, 1,  0, 1,  0,
    -1, -1, 1,  1,  -1, -1, 1, -1, 1, -1, -1, 1,  -1, -1, -1, 1, -1, -1,
};

std::vector<GLfloat> normal_buffer_data = {
    0.0f,    0.4472f, 0.8944f,  0.0f,    0.4472f, 0.8944f,  0.0f,
    0.4472f, 0.8944f, 0.8944f,  0.4472f, 0.0f,    0.8944f,  0.4472f,
    0.0f,    0.8944f, 0.4472f,  0.0f,    0.0f,    0.4472f,  -0.8944f,
    0.0f,    0.4472f, -0.8944f, 0.0f,    0.4472f, -0.8944f, -0.8944f,
    0.4472f, 0.0f,    -0.8944f, 0.4472f, 0.0f,    -0.8944f, 0.4472f,
    0.0f,    0,       -1,       0,       0,       -1,       0,
    0,       -1,      0,        0,       -1,      0,        0,
    -1,      0,       0,        -1,      0,
}; */

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
    if (glewInit())
    {
        std::cerr << "Error while initializing glew" << std::endl;
        return false;
    }
    return true;
}

// Init OpenGL
void init_GL()
{
    // Make OpenGL vigilant on object overwrite with the z buffer
    glEnable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();

    // Backface culling
    // glEnable(GL_CULL_FACE);
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
bool init_object(const mygl::program* p)
{
    // Get the get the location in the program of a named attribute here
    // position
    // return -1 if name is not an active attribute (not found)
    GLint vertex_location = glGetAttribLocation(p->program_id, "position");
    TEST_OPENGL_ERROR();
    if (vertex_location == -1)
    {
        std::cerr << "Attribute 'position' not found in shader" << std::endl;
        return false;
    }

    GLint normal_location = glGetAttribLocation(p->program_id, "normal");
    TEST_OPENGL_ERROR();
    if (normal_location == -1)
    {
        std::cerr << "Attribute 'position' not found in shader" << std::endl;
        return false;
    }

    GLint color_location = glGetAttribLocation(p->program_id, "color");
    TEST_OPENGL_ERROR();
    if (color_location == -1)
    {
        std::cerr << "Attribute 'color' not found in shader" << std::endl;
        return false;
    }

    GLint texture_location = glGetAttribLocation(p->program_id, "uv");
    TEST_OPENGL_ERROR();
    if (color_location == -1)
    {
        std::cerr << "Attribute 'uv' not found in shader" << std::endl;
        return false;
    }

    // Generate 1 vertex array
    // The name is stored in vao_id (global variable)
    GLuint vbo_ids[4];
    glGenVertexArrays(1, &vao_id);
    TEST_OPENGL_ERROR();

    // Start recording the VAO
    // Bindings below will be remembered
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // Generate 4 VBO
    // One for the normal and the other for the positions
    glGenBuffers(4, vbo_ids);
    TEST_OPENGL_ERROR();

    // Bind the first VBO as the active one for GL_ARRAY_BUFFER operations
    // glBufferData & glVertexAttribPointer are calling VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[0]);
    TEST_OPENGL_ERROR();

    // Upload the CPU vector in GPU memory
    // GL_STATIC_DRAW hints the GPU that you intend to draw multiples times and
    // he will choose the best way to optimize your code using that
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(GLfloat),
                 vertex_buffer_data.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    // Tell the VAO how to read in the vertex_location
    // read 3 GL_FLOAT per vectex
    // with normalized: GL_FALSE
    // stride: NO
    glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    TEST_OPENGL_ERROR();

    // Enable the attribute VBO for the position attribute
    glEnableVertexAttribArray(vertex_location);
    TEST_OPENGL_ERROR();

    // Same process for the second VBO
    // Make the VBO active for GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[1]);
    TEST_OPENGL_ERROR();

    // Upload the CPU vector in GPU memory while giving GL_STATIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, normal_buffer_data.size() * sizeof(GLfloat),
                 normal_buffer_data.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    // How to read in the normal_location
    glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    TEST_OPENGL_ERROR();

    // Enable the attribute VBO for the position attribute
    glEnableVertexAttribArray(normal_location);
    TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[2]);
    TEST_OPENGL_ERROR();

    // Upload the CPU vector in GPU memory while giving GL_STATIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, color_buffer_data.size() * sizeof(GLfloat),
                 color_buffer_data.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    // How to read in the normal_location
    glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    TEST_OPENGL_ERROR();

    // Enable the attribute VBO for the position attribute
    glEnableVertexAttribArray(color_location);
    TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[3]);
    TEST_OPENGL_ERROR();

    // Upload the CPU vector in GPU memory while giving GL_STATIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, texture_buffer_data.size() * sizeof(GLfloat),
                 texture_buffer_data.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    // How to read in the normal_location
    glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
    TEST_OPENGL_ERROR();

    // Enable the attribute VBO for the position attribute
    glEnableVertexAttribArray(texture_location);
    TEST_OPENGL_ERROR();

    // Unbind the VAO while setting 0 (nothing)
    glBindVertexArray(0);
    return true;
}

bool init_POV(const mygl::program* p)
{
    mygl::Matrix4 view =
        mygl::lookat(0.0f, 1.5f, -4.0f, 1.0f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);

    mygl::Matrix4 proj = mygl::frustum(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 100.0f);

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

    // set light position
    glUniform3f(light_loc, 1.0f, 1.0f, 1.0f);
    TEST_OPENGL_ERROR();

    return true;
}

bool init_GLFW()
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS

    window = glfwCreateWindow(1024, 1024, "TestOpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    return true;
}

void display(const mygl::program* p)
{
    // Clear the color and the depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TEST_OPENGL_ERROR();

    // Activate the VAO
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // Get the uniformLocation of object_color
    // GLint color_loc = glGetUniformLocation(p->program_id, "object_color");
    // TEST_OPENGL_ERROR();
    // if (color_loc == -1)
    // {
    //     std::cerr << "Uniform 'color_location' not found" << std::endl;
    // }

    // // Upload a warm color fot the object
    // // This variable is used for the object as a fiffuse color in the
    // // lighting
    // glUniform3f(color_loc, 0.95f, 0.4f, 0.f);
    // TEST_OPENGL_ERROR();

    // Set primitives
    glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_data.size() / 3);
    TEST_OPENGL_ERROR();

    // Unbind the VAO
    glBindVertexArray(0);
    TEST_OPENGL_ERROR();

    // Swaps the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool load_obj(const std::string& path)
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

    vertex_buffer_data.clear();
    normal_buffer_data.clear();

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
                    vertex_buffer_data.push_back(
                        attrib.vertices[3 * idx.vertex_index + 0]);
                    vertex_buffer_data.push_back(
                        attrib.vertices[3 * idx.vertex_index + 1]);
                    vertex_buffer_data.push_back(
                        attrib.vertices[3 * idx.vertex_index + 2]);

                    // Normal (fall back to up-vector if absent)
                    if (idx.normal_index >= 0)
                    {
                        normal_buffer_data.push_back(
                            attrib.normals[3 * idx.normal_index + 0]);
                        normal_buffer_data.push_back(
                            attrib.normals[3 * idx.normal_index + 1]);
                        normal_buffer_data.push_back(
                            attrib.normals[3 * idx.normal_index + 2]);
                    }
                    else
                    {
                        normal_buffer_data.push_back(0.f);
                        normal_buffer_data.push_back(1.f);
                        normal_buffer_data.push_back(0.f);
                    }

                    if (idx.texcoord_index >= 0)
                    {
                        texture_buffer_data.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 0]);
                        texture_buffer_data.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 1]);
                    }

                    color_buffer_data.push_back(
                        attrib.colors[3 * idx.vertex_index + 0]);
                    color_buffer_data.push_back(
                        attrib.colors[3 * idx.vertex_index + 1]);
                    color_buffer_data.push_back(
                        attrib.colors[3 * idx.vertex_index + 2]);
                }
            }
            index_offset += fv;
        }
    }

    return !vertex_buffer_data.empty();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Wrong usage: ./main <file.obj>\n";
        return 2;
    }

    if (!load_obj(argv[1]))
    {
        std::cerr << "Wrong obj file\n";
        return 3;
    }

    // Create GL window and context

    init_GLFW();

    // Loads GL functions pointers
    if (!init_glew())
        return 1;

    // Set global rendering state
    init_GL();

    mygl::program* p = nullptr;

    if (!init_shaders(&p))
        return 1;
    if (!init_object(p))
        return 1;
    if (!init_POV(p))
        return 1;

    while (!glfwWindowShouldClose(window))
        display(p);

    delete p;
    glfwTerminate();

    return 0;
}
