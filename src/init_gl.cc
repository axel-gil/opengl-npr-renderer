#include "init_gl.hh"

#include "gl_error.hh"

#include <iostream>

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

// Init the global
void init_object(const std::vector<GLfloat>& obj_buffer, GLuint* vao_id)
{
    // Generate 1 vertex array
    // The name is stored in vao_id (global variable)
    GLuint vbo_ids[1];
    glGenVertexArrays(1, vao_id);
    TEST_OPENGL_ERROR(); // Start recording the VAO
    // Bindings below will be remembered
    glBindVertexArray(*vao_id);
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
}

bool init_GLFW(GLFWwindow** window)
{
#ifndef __APPLE__
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
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
