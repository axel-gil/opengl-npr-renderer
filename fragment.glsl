#version 450 core

// Interpolated values from vertex shader
in vec3 frag_normal;
in vec3 frag_pos_cam;

// Uniforms uploaded by init_POV()
uniform vec3 object_color; // base diffuse color of the object
uniform vec3 light_dir;    // light direction in world space (non-normalized is fine)

out vec4 frag_color;

void main()
{
    // Re-normalize after interpolation (interpolated normals lose unit length)
    vec3 N = normalize(frag_normal);

    // Light direction in camera space (model_view is a rotation, so mat3 works)
    // We negate because light_dir points *toward* the light, but we need the
    // direction *from* the surface to the light.
    vec3 L = normalize(light_dir);

    // --- Ambient ---
    // A small constant term so faces pointing away from the light aren't pure black.
    float ambient_strength = 0.15;
    vec3 ambient = ambient_strength * object_color;

    // --- Diffuse (Lambertian) ---
    // max(0, N·L): surfaces facing the light are bright, back-faces are 0.
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * object_color;

    // --- Specular (Blinn-Phong) ---
    // The view direction: from the fragment toward the camera (camera is at origin
    // in camera space, so view dir = -frag_pos_cam).
    vec3 V = normalize(-frag_pos_cam);
    // Halfway vector between light and view — Blinn-Phong approximation.
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0); // 32 = shininess exponent
    vec3 specular = spec * vec3(1.0); // white specular highlight

    // Combine all three lighting components
    vec3 color = ambient + diffuse + 0.5 * specular;

    frag_color = vec4(color, 1.0);
}
