#version 450

layout(location=0) in vec2 a_position;
layout(location=1) in vec2 a_tex_coords;

layout(location=0) out vec2 v_tex_coords;

layout(set=1, binding=0)
uniform Uniforms {
    mat4 u_model;
};

void main() {
    v_tex_coords = a_tex_coords;
    gl_Position = u_model * vec4(a_position, 0., 1.); // 3.
}
