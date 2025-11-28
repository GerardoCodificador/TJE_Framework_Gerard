uniform sampler2D u_texture;
varying vec2 v_uv;
void main() {
    vec2 uv = v_uv;
    vec4 color = vec4(vec3(texture(u_texture, uv).r));
    gl_FragColor = color;
}