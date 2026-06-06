#define GL_SILENCE_DEPRECATION
#include "program.hh"

#include <fstream>
#include <sstream>
#include <vector>

#include "OpenGL/gl3.h"

namespace mygl
{
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

    static bool load_and_compile_shader(program* p, const GLenum shader_type,
                                        const std::string& filename,
                                        GLuint& shader_id)
    {
        // Load the file content
        std::string src = load(filename);
        const GLchar* sources[] = { src.c_str() };
        const GLint lens[] = { static_cast<GLint>(src.size()) };

        // Create a shader of type shader_type
        shader_id = glCreateShader(shader_type);

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
            std::stringstream ss;

            ss << "FAILURE can not compile shader " << filename << ": " << log
               << std::endl;

            p->logs = ss.str();
            p->ready = false;

            std::free(log);

            // Remove the shader
            glDeleteShader(shader_id);
            return false;
        }
        return true;
    }

    // Link the shaders to a program
    static bool attach_and_link_program(program* p,
                                        const std::vector<GLuint>& shaders,
                                        GLuint& prog)
    {
        // Create the program
        prog = glCreateProgram();

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
            p->logs = ss.str();
            p->ready = false;
            std::free(log);

            // Remove the shader
            glDeleteProgram(prog);
            prog = 0;
            return false;
        }
        return true;
    }

    program* program::makeprogram(const std::string& vertexshadersrc,
                                  const std::string& fragmentshadersrc)
    {
        auto p = new program;

        if (!load_and_compile_shader(p, GL_VERTEX_SHADER, vertexshadersrc,
                                     p->vertex_shader))
            return p;

        if (!load_and_compile_shader(p, GL_FRAGMENT_SHADER, fragmentshadersrc,
                                     p->fragment_shader))
            return p;

        std::vector<GLuint> shaders = { p->vertex_shader, p->fragment_shader };

        if (!attach_and_link_program(p, shaders, p->program_id))
            return p;

        for (auto s : shaders)
        {
            // Then detach the shader from the program
            glDetachShader(p->program_id, s);

            // Then delete the shader
            glDeleteShader(s);
        }

        p->ready = true;
        return p;
    }

    const std::string& program::get_log() const
    {
        return logs;
    }

    bool program::is_ready() const
    {
        return ready;
    }

    void program::use() const
    {
        glUseProgram(program_id);
    }
} // namespace mygl
