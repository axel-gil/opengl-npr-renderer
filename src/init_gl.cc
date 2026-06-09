#include "init_gl.hh"

#include "gl_error.hh"
#include "matrix4.hh"
#include "transformation.hh"

#include <iostream>
#include <cmath>

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
bool init_object(const std::vector<GLfloat>& obj_buffer, GLuint* vao_id)
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
    return true;
}

bool init_POV(const mygl::program* p)
{
    mygl::Matrix4 view =
        mygl::lookat(0.0f, 1.5f, -4.0f, 1.0f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);

    // FOV of 90
    float fovy = 90.0f * M_PI / 180.0f;
    float aspect = 1920.0 / 1080;
    float znear = 0.1f, zfar = 10000.0f;

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
        glUniform3f(light_loc, 1.0f, 1.0f, 1.0f);
    }

    // Set light position
    TEST_OPENGL_ERROR();

    return true;
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
