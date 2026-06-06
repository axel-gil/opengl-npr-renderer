#version 410 core

// Per-vertex position from VBO (in object/world space)
in vec3 position;

// Per-vertex normal from VBO (in world space)
in vec3 normal;

// Matrices uploaded by init_POV()
uniform mat4 model_view;   // world -> camera space
uniform mat4 projection;   // camera space -> clip space

// Pass the normal and the position (in camera space) to the fragment shader.
// 'out' variables are interpolated across the triangle's surface automatically.
out vec3 frag_normal;
out vec3 frag_pos_cam; // position in camera space (used to compute view direction)

void main()
{
    // Transform position into camera space (needed for lighting in frag shader)
    vec4 pos_cam = model_view * vec4(position, 1.0);
    frag_pos_cam = pos_cam.xyz;

    // Transform the normal into camera space.
    // We use the upper-left 3x3 of model_view (no translation).
    // For a pure rotation+translation matrix this is correct.
    frag_normal = mat3(model_view) * normal;

    // Final clip-space position (what OpenGL actually uses for rasterization)
    gl_Position = projection * pos_cam;
}
