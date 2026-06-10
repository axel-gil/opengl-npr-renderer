#include "program.hh"

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "program.hh"
#include "gl_error.hh"
#include "matrix4.hh"
#include "transformation.hh"

static GLuint load_and_compile_shader(Program* p, const GLenum shader_type,
                                      const std::string& filename);

static GLuint attach_and_link_program(Program* p,
                                      const std::vector<GLuint>& shaders);

Program::Program()
    : program_id_(0)
    , vertex_shader_(0)
    , fragment_shader_(0)
    , ready_state_(false)
{}

bool Program::init_shaders(std::string shader_prefix)
{
    if (shader_prefix.size() > 0)
    {
        shader_prefix.append("_");
    }

    std::string vertex_shader_file = "shaders/" + shader_prefix + "vertex.shd";
    std::string fragment_shader_file =
        "shaders/" + shader_prefix + "fragment.shd";

    GLuint shader_id;

    vertex_shader_ =
        load_and_compile_shader(this, GL_VERTEX_SHADER, vertex_shader_file);
    if (vertex_shader_ == 0)
    {
        std::cerr << "Error while loading " << vertex_shader_file << "\n ";
        return false;
    }

    fragment_shader_ =
        load_and_compile_shader(this, GL_FRAGMENT_SHADER, fragment_shader_file);
    if (fragment_shader_ == 0)
    {
        std::cerr << "Error while loading " << fragment_shader_file << "\n ";
        return false;
    }

    std::vector<GLuint> shaders = { vertex_shader_, fragment_shader_ };

    program_id_ = attach_and_link_program(this, shaders);
    if (program_id_ == 0)
        return false;

    for (auto s : shaders)
    {
        glDetachShader(program_id_, s);
        glDeleteShader(s);
    }

    ready_state_ = true;
    use();
    return true;
}

void Program::init_POV()
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
    GLint view_loc = glGetUniformLocation(get_program_id(), "model_view");
    TEST_OPENGL_ERROR();

    GLint proj_loc = glGetUniformLocation(get_program_id(), "projection");
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
    GLint light_loc = glGetUniformLocation(get_program_id(), "light_dir");
    TEST_OPENGL_ERROR();
    if (light_loc != -1)
    {
        glUniform3f(light_loc, 1.0f, 1.0f, 1.0f);
    }

    // Set light position
    TEST_OPENGL_ERROR();
}

static std::string load(const std::string& filename)
{
    std::ifstream input_src_file(filename, std::ios::in);
    std::string ligne;
    std::string file_content = "";

    if (input_src_file.fail())
        return "";

    while (getline(input_src_file, ligne))
        file_content = file_content + ligne + "\n";

    file_content += '\0';
    input_src_file.close();
    return file_content;
}

static GLuint load_and_compile_shader(Program* p, const GLenum shader_type,
                                      const std::string& filename)
{
    // Load the file content
    std::string src = load(filename);
    const GLchar* sources[] = { src.c_str() };
    const GLint lens[] = { static_cast<GLint>(src.size()) };

    // Create a shader of type shader_type
    GLuint shader_id = glCreateShader(shader_type);

    // Loads sources in the shader
    glShaderSource(shader_id, 1, sources, lens);

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
        std::stringstream ss;

        ss << "FAILURE can not compile shader " << filename << ": " << log
           << std::endl;

        p->set_logs(ss.str());
        p->set_ready_state(false);

        std::free(log);

        // Remove the shader
        glDeleteShader(shader_id);
        return 0;
    }
    return shader_id;
}

// Link the shaders to a program
static GLuint attach_and_link_program(Program* p,
                                      const std::vector<GLuint>& shaders)
{
    // Create the program
    GLuint prog = glCreateProgram();

    for (auto s : shaders)
    {
        // Add the shaders to the program
        glAttachShader(prog, s);
    }

    // Link the shaders attached to the program
    // GL_VERTEX_SHADER is used to create an executable that will run on the
    // vertex processor
    // GL_FRAGMENT_SHADER is used create an executable that will run on
    // fragment processor
    glLinkProgram(prog);

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
        std::stringstream ss;

        ss << "Program link error: " << log << std::endl;
        p->set_logs(ss.str());
        p->set_ready_state(false);
        std::free(log);

        // Remove the shader
        glDeleteProgram(prog);
        prog = 0;
    }
    return prog;
}

GLuint Program::get_program_id() const
{
    return program_id_;
};

GLuint Program::get_vertex() const
{
    return vertex_shader_;
}

GLuint Program::get_fragment() const
{
    return fragment_shader_;
}

bool Program::get_ready_state() const
{
    return ready_state_;
}

const std::string& Program::get_logs() const
{
    return logs_;
};

void Program::set_program_id(GLuint program_id)
{
    program_id_ = program_id;
}

void Program::set_vertex_shader(GLuint vertex_shader)
{
    vertex_shader_ = vertex_shader;
}

void Program::set_fragment_shader(GLuint fragment_shader)
{
    fragment_shader_ = fragment_shader;
}

void Program::set_ready_state(bool state)
{
    ready_state_ = state;
}

void Program::set_logs(std::string log)
{
    logs_ += log;
};

void Program::use() const
{
    glUseProgram(program_id_);
}
