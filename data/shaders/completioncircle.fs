#version 330 core

out vec4 FragColor;

in vec2 v_texcoord;

uniform vec2 u_center;  
uniform vec2 u_size;    
uniform float u_percent;
uniform vec4 u_color; 
uniform vec3 u_colorB;  
void main() {
    vec2 rel = (v_texcoord - u_center) / u_size;

    float dist = length(rel);

    if (dist > 1.0) {
        FragColor = vec4(0.0);
        return;
    }
    float angle = atan(rel.y, rel.x);      
    angle = (angle + 3.14159265) / (2.0 * 3.14159265); 
    vec3 colorA = u_color.rgb;
    vec3 finalColor = (angle <= u_percent) ? colorA : u_colorB;
    FragColor = vec4(0.0);
    if(u_percent>0.0)FragColor = vec4(finalColor, 1.0);

}