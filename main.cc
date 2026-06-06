/************************************************************************/
/*                                                                      */
/* (c) J. Fabrizio                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/

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
#include "object_vbo.hh"

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
GLuint program_id;
GLFWwindow* window;

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

// Get file content as string
std::string load(const std::string& filename)
{
    // Create a inputfile stream
    std::ifstream f(filename, std::ios::in);
    if (f.fail())
    {
        std::cerr << "Cannot load: " << filename << std::endl;
        return "";
    }

    // Get each lines in a string
    std::string content;
    std::string line;
    while (getline(f, line))
        content += line + "\n";
    content += '\0';
    return content;
}

// Compiles the shader
bool load_and_compile_shader(const GLenum shader_type,
                             const std::string& filename, GLuint& shader_id)
{
    // Load the file content
    std::string src = load(filename);
    const GLchar* sources[] = { src.c_str() };
    const GLint lens[] = { static_cast<GLint>(src.size()) };

    // Create a shader of type shader_type
    shader_id = glCreateShader(shader_type);
    TEST_OPENGL_ERROR();

    // Loads sources in the shader
    glShaderSource(shader_id, 1, sources, lens);
    TEST_OPENGL_ERROR();

    // Compiles the shader
    glCompileShader(shader_id);

    // Get the GL_COMPILE_STATUS parameter
    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        // Get the GL_INFO_LOG_LENGTH
        GLint log_size;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_size);

        // Allocate a string for the logs, fetch and display them
        char* log = (char*)std::malloc(log_size + 1);
        glGetShaderInfoLog(shader_id, log_size, &log_size, log);
        std::cerr << "Shader compile error (" << filename << "): " << log
                  << std::endl;
        std::free(log);

        // Remove the shader
        glDeleteShader(shader_id);
        return false;
    }
    return true;
}

// Link the shaders to a program
bool attach_and_link_program(const std::vector<GLuint>& shaders, GLuint& prog)
{
    // Create the program
    prog = glCreateProgram();
    TEST_OPENGL_ERROR();

    for (auto s : shaders)
    {
        // Add the shaders to the program
        glAttachShader(prog, s);
        TEST_OPENGL_ERROR();
    }

    // Link the shaders attached to the program
    // GL_VERTEX_SHADER is used to create an executable that will run on the
    // vertex processor
    // GL_FRAGMENT_SHADER is used create an executable that will run on fragment
    // processor
    glLinkProgram(prog);
    TEST_OPENGL_ERROR();

    // Get the GL_LINK_STATUS
    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        // Get the GL_INFO_LOG_LENGTH
        GLint log_size;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_size);

        // Allocate a string for the logs, fetch and display them
        char* log = (char*)std::malloc(log_size + 1);
        glGetProgramInfoLog(prog, log_size, &log_size, log);
        std::cerr << "Program link error: " << log << std::endl;
        std::free(log);

        // Remove the shader
        glDeleteProgram(prog);
        prog = 0;
        return false;
    }
    return true;
}

// Init the shaders
bool init_shaders()
{
    GLuint vert_id;
    GLuint frag_id;

    // Loads the differents shaders
    if (!load_and_compile_shader(GL_VERTEX_SHADER, "vertex.glsl", vert_id))
        return false;
    if (!load_and_compile_shader(GL_FRAGMENT_SHADER, "fragment.glsl", frag_id))
        return false;

    // List the shaders and links them to a program
    std::vector<GLuint> shaders = { vert_id, frag_id };
    if (!attach_and_link_program(shaders, program_id))
        return false;

    for (auto s : shaders)
    {
        // Then detach the shader from the program
        glDetachShader(program_id, s);
        TEST_OPENGL_ERROR();

        // Then delete the shader
        glDeleteShader(s);
        TEST_OPENGL_ERROR();
    }

    // Use the program in the rendering state
    glUseProgram(program_id);
    TEST_OPENGL_ERROR();
    return true;
}

// Init the global
bool init_object()
{
    // Get the get the location in the program of a named attribute here
    // position
    // return -1 if name is not an active attribute (not found)
    GLint vertex_location = glGetAttribLocation(program_id, "position");
    TEST_OPENGL_ERROR();
    if (vertex_location == -1)
    {
        std::cerr << "Attribute 'position' not found in shader" << std::endl;
        return false;
    }

    GLint normal_location = glGetAttribLocation(program_id, "normal");
    TEST_OPENGL_ERROR();
    if (normal_location == -1)
    {
        std::cerr << "Attribute 'position' not found in shader" << std::endl;
        return false;
    }

    // Generate 1 vertex array
    // The name is stored in vao_id (global variable)
    GLuint vbo_ids[2];
    glGenVertexArrays(1, &vao_id);
    TEST_OPENGL_ERROR();

    // Start recording the VAO
    // Bindings below will be remembered
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // Generate 2 VBO
    // One for the normal and the other for the positions
    glGenBuffers(2, vbo_ids);
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

    // Unbind the VAO while setting 0 (nothing)
    glBindVertexArray(0);
    return true;
}

bool init_POV()
{
    mygl::Matrix4 view =
        mygl::lookat(0.0f, 1.5f, -4.0f, 1.0f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);

    mygl::Matrix4 proj = mygl::frustum(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 100.0f);

    // Get the uniformLocation in the program with a named uniform variable
    // Return -1 if the variable doesn't exist
    GLint view_loc = glGetUniformLocation(program_id, "model_view");
    TEST_OPENGL_ERROR();

    GLint proj_loc = glGetUniformLocation(program_id, "projection");
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
    GLint light_loc = glGetUniformLocation(program_id, "light_dir");
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

int main(int argc, char* argv[])
{
    // Create GL window and context
    // init_glut(argc, argv);
    (void)argc;
    (void)argv;
    init_GLFW();

    // Loads GL functions pointers
    if (!init_glew())
        return 1;

    // Set global rendering state
    init_GL();

    if (!init_shaders())
        return 1;
    if (!init_object())
        return 1;
    if (!init_POV())
        return 1;

    while (!glfwWindowShouldClose(window))
    {
        // Clear the color and the depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TEST_OPENGL_ERROR();

        // Activate the VAO
        glBindVertexArray(vao_id);
        TEST_OPENGL_ERROR();

        // Get the uniformLocation of object_color
        GLint color_loc = glGetUniformLocation(program_id, "object_color");
        TEST_OPENGL_ERROR();
        if (color_loc == -1)
        {
            std::cerr << "Uniform 'color_location' not found" << std::endl;
        }

        // Upload a warm color fot the object
        // This variable is used for the object as a fiffuse color in the lighting
        glUniform3f(color_loc, 0.95f, 0.4f, 0.f);
        TEST_OPENGL_ERROR();

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

    glfwTerminate();
    // glutMainLoop();
    return 0;
}
