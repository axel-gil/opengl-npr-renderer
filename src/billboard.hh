#pragma once

#include <type_traits>
#include <vector>
#include <GL/glew.h>
#include "fire.hh"
#include "particle.hh"

template <typename T>
concept ParticleDerived = std::is_base_of_v<Particle, T>;

template <ParticleDerived T>
class Billboard
{
public:
    Billboard(std::vector<GLfloat> vertex_buffer_data, size_t max_particles);
    bool init_particles();
    void update_particles();
    bool display();

private:
    std::vector<T> particles;

    std::vector<GLfloat> g_vertex_buffer_data;
    std::vector<GLfloat> g_particule_position_size_data;
    std::vector<GLubyte> g_particule_color_data;

    size_t current_index_ = 0;
    size_t alive_count_ = 0;
    size_t particle_per_frame_;
    size_t max_particles_;
    float last_time;

    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;

    GLuint vao_id;
};
