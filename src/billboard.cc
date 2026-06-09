#include "billboard.hh"
#include <GLFW/glfw3.h>
#include "program.hh"
#include "vector3.hh"
#include "gl_error.hh"
#include "fire.hh"

static const std::vector<GLfloat> g_vertex_buffer_data = {
    -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f,
};

template <ParticleDerived T>
Billboard<T>::Billboard(size_t max_particles)
    : particles(max_particles)
    , g_particule_position_size_data(max_particles * 4, 0)
    , g_particule_color_data(max_particles * 4, 0)
    , particle_per_frame_(2)
    , max_particles_(max_particles)
    , last_time(static_cast<float>(glfwGetTime()))
{
    glGenVertexArrays(1, &vao_id);
    TEST_OPENGL_ERROR();
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    glGenBuffers(1, &billboard_vertex_buffer);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat),
                 g_vertex_buffer_data.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLfloat),
                 g_particule_position_size_data.data(), GL_STREAM_DRAW);
    TEST_OPENGL_ERROR();

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLubyte),
                 g_particule_color_data.data(), GL_STREAM_DRAW);
    TEST_OPENGL_ERROR();
}

template <ParticleDerived T>
bool Billboard<T>::init_particles()
{
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // billboard vertices
    glEnableVertexAttribArray(0);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    TEST_OPENGL_ERROR();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
    TEST_OPENGL_ERROR();

    // particles position
    glEnableVertexAttribArray(1);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    TEST_OPENGL_ERROR();
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));
    TEST_OPENGL_ERROR();

    // particles color
    glEnableVertexAttribArray(2);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    TEST_OPENGL_ERROR();
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,
                          static_cast<void*>(0));
    TEST_OPENGL_ERROR();

    glVertexAttribDivisor(0, 0); // billboard vertices -> per vertex
    TEST_OPENGL_ERROR();
    glVertexAttribDivisor(1, 1); // instances positions -> per instance
    TEST_OPENGL_ERROR();
    glVertexAttribDivisor(2, 1); // instances colors -> per instance
    TEST_OPENGL_ERROR();

    return true;
}

template <ParticleDerived T>
void Billboard<T>::update_particles()
{
    float current_time = static_cast<float>(glfwGetTime());
    float dt = current_time - last_time;
    last_time = current_time;

    for (size_t i = 0; i < particle_per_frame_; i++)
    {
        particles[current_index_].spawn();
        current_index_ = (current_index_ + 1) % max_particles_;
    }

    alive_count_ = 0;

    for (size_t i = 0; i < max_particles_; i++)
    {
        T& p = particles[i];

        if (!p.is_alive())
            continue;

        p.update(dt);

        // particle is dying this frame
        if (!p.is_alive())
            continue;

        // position
        g_particule_position_size_data[alive_count_ * 4 + 0] =
            p.get_pos().get_x();
        g_particule_position_size_data[alive_count_ * 4 + 1] =
            p.get_pos().get_y();
        g_particule_position_size_data[alive_count_ * 4 + 2] =
            p.get_pos().get_z();

        // size
        g_particule_position_size_data[alive_count_ * 4 + 3] = p.get_size();

        // color
        g_particule_color_data[alive_count_ * 4 + 0] = p.get_color().r;
        g_particule_color_data[alive_count_ * 4 + 1] = p.get_color().g;
        g_particule_color_data[alive_count_ * 4 + 2] = p.get_color().b;
        g_particule_color_data[alive_count_ * 4 + 3] = p.get_color().a;

        alive_count_++;
    }

    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, max_particles_ * 4 * sizeof(GLfloat), nullptr,
                 GL_STREAM_DRAW); // Buffer orphaning
    TEST_OPENGL_ERROR();
    glBufferSubData(GL_ARRAY_BUFFER, 0, alive_count_ * 4 * sizeof(GLfloat),
                    g_particule_position_size_data.data());
    TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, max_particles_ * 4 * sizeof(GLubyte), nullptr,
                 GL_STREAM_DRAW); // Buffer orphaning
    TEST_OPENGL_ERROR();
    glBufferSubData(GL_ARRAY_BUFFER, 0, alive_count_ * 4 * sizeof(GLubyte),
                    g_particule_color_data.data());
    TEST_OPENGL_ERROR();
}

template <ParticleDerived T>
bool Billboard<T>::display()
{
    glBindVertexArray(vao_id);
    TEST_OPENGL_ERROR();

    // enable a channel for transparency
    glEnable(GL_BLEND);
    TEST_OPENGL_ERROR();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    TEST_OPENGL_ERROR();

    // disable depth mask so that transparencies can compound
    // Ex:
    // - A: depth 5 and a = 0.2
    // - B: depth 6 and a = 0.2
    //
    // it will draws both A and B and not stop at B
    glDepthMask(GL_FALSE);
    TEST_OPENGL_ERROR();

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, alive_count_);
    TEST_OPENGL_ERROR();

    // re-enable depth mask
    glDepthMask(GL_TRUE);

    return true;
}

template class Billboard<Fire>;
