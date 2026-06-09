#include "object.hh"
#include "gl_error.hh"
#include "matrix4.hh"
#include "transformation.hh"

#include <cmath>

Object::Object(std::vector<GLfloat> buffer, GLuint texture_id, Program* p,
               Camera* c)
    : buffer(buffer)
    , texture_id(texture_id)
    , program(p)
    , camera(c)
{}

bool Object::init()
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

    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat),
                 buffer.data(), GL_STATIC_DRAW);
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

static mygl::Matrix4 update(float time)
{
    float flicker_y = 1 + 0.15f * sin(time * 15.0f);
    float flicker_x = 1 + 0.05f * sin(time * 17.0f);

    mygl::Matrix4 t = mygl::translate(0.0f, 0.5f, 0.0f);
    mygl::Matrix4 s = mygl::scale(flicker_x, flicker_y, flicker_x);
    t *= s;

    return t;
}

void Object::bounce(float time) const
{
    GLint model_loc =
        glGetUniformLocation(program->get_program_id(), "model_view");
    TEST_OPENGL_ERROR();

    mygl::Matrix4 flame_model_matrix = camera->get_view();
    flame_model_matrix *= update(time);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE,
                       flame_model_matrix.get_data().data());
    TEST_OPENGL_ERROR();
}

void Object::display() const
{
    // use program / shaders of object
    program->use();
    TEST_OPENGL_ERROR();

    // Activate the VAO
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // Set the textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(glGetUniformLocation(program->get_program_id(), "tex_diffuse"),
                0);

    // Set primitives
    glDrawArrays(GL_TRIANGLES, 0, buffer.size() / 11);
    TEST_OPENGL_ERROR();

    // Unbind the VAO
    glBindVertexArray(0);
    TEST_OPENGL_ERROR();
}
