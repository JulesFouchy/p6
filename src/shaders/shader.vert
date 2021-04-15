#version 450

layout(location=0) in vec2 a_position;
layout(location=1) in vec2 a_tex_coords;

layout(location=0) out vec2 v_tex_coords;

layout(set=0, binding=0)
uniform Uniforms {
    mat4 u_model;
    float u_aspect_ratio;
};

void main() {
    v_tex_coords = a_tex_coords;
    gl_Position = u_model * vec4(a_position, 0., 1.);
    gl_Position.x /= u_aspect_ratio;
}
