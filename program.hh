#pragma once

#include <string>

#include <GL/glew.h>

namespace mygl
{
    class program
    {
    public:
        static program* makeprogram(const std::string& vertexshadersrc,
                                    const std::string& fragmentshadersrc);
        const std::string& get_log() const;
        bool is_ready() const;

        void use() const;

    public:
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program_id;
        bool ready;
        std::string logs;
    };
} // namespace mygl
