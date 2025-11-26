#version 330 core

uniform sampler2D u_texture;

uniform vec4 u_color;
in vec2 v_texcoord;
out vec4 fragColor;

void main() {
    vec4 color = u_color;

    if(color.a < 0.01)
        color = vec4(1.0);

    fragColor = color * texture(u_texture, v_texcoord);
}