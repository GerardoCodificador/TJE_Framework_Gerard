#version 330 core

in vec3 a_vertex;   // <--- match your C++ expectation
in vec2 a_uv;

out vec2 v_texcoord;

void main() {
    v_texcoord = vec2(a_uv.x, 1.0 - a_uv.y);
    vec2 pos = a_vertex.xy * 2.0 - 1.0;
    gl_Position = vec4(pos, 0.0, 1.0);
}