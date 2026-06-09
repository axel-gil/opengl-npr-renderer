#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

#ifdef __APPLE__
#    include <OpenGL/gl3.h>
#endif

class Program
{
public:
    Program();
    bool init_shaders(std::string shader_prefix);
    void init_POV();
    Program* makeprogram(const std::string& vertexshadersrc,
                         const std::string& fragmentshadersrc);
    void use() const;

    GLuint get_program_id() const;
    GLuint get_vertex() const;
    GLuint get_fragment() const;
    bool get_ready_state() const;
    const std::string& get_logs() const;

    void set_program_id(GLuint program_id);
    void set_vertex_shader(GLuint vertex_shader);
    void set_fragment_shader(GLuint fragment_shader);
    void set_ready_state(bool state);
    void set_logs(std::string log);

private:
    GLuint program_id_;
    GLuint vertex_shader_;
    GLuint fragment_shader_;
    bool ready_state_;
    std::string logs_;
};
