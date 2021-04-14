#version 450

layout(location=0) in vec2 v_tex_coords;
layout(location=0) out vec4 f_color;

void main() {
    if (length(v_tex_coords - vec2(0.5)) > 0.5)
        discard;
    f_color = vec4(v_tex_coords, 0., 1.);
}