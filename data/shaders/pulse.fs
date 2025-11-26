

float sdSphere(vec3 p, float s) {
    return length(p) - s;
}

float when_lt(float left_side, float right_side) {
    return max(sign(right_side - left_side), 0.0);
}

float when_gt(float left_side, float right_side) {
    return max(sign(left_side - right_side), 0.0);
}

// ---------------------------
// UNIFORMS
// ---------------------------
uniform vec4 u_color;            // color base si no hay textura
uniform vec3 u_pulse_color;      // color del pulso
uniform float u_pulse_width;     // ancho del pulso
uniform vec3 u_pulse_center;     // posición del jugador / centro de luz
uniform float u_pulse_radius;    // radio del pulso
uniform int u_pulse_active;      // 1=activo, 0=inactivo
uniform int u_is_texture;        // 1=usar textura, 0=color
uniform sampler2D u_texture;     // textura base
uniform int u_isinstanced;  	// 1=instanciado, 0=no instanciado
// ---------------------------
// VARYINGS
// ---------------------------
varying vec2 v_uv;
varying vec3 v_world_position;
varying vec4 v_color; 
varying vec3 v_normal;          
void main() {
    vec2 uv = v_uv;
    vec3 world_position = v_world_position;


    vec4 final_color = u_color;
	if(!u_isinstanced==1){
		final_color =mix(v_color,u_color,0.5);
	}
    if(u_is_texture == 0){
        final_color = texture(u_texture, uv);
    }

    // ---------------------------
    // 2) ILUMINACIÓN RADIAL DESDE EL PLAYER
    // ---------------------------
    float max_light_distance = u_pulse_radius;  // hasta dónde llega la luz
    float min_light = 0.1;           // oscuridad mínima

    float light_dist = length(world_position - u_pulse_center);
    float light_factor = 1.0 - clamp(light_dist / max_light_distance, 0.0, 1.0);
    light_factor = mix(min_light, 1.0, light_factor);

    final_color.rgb *= light_factor;

 	vec3 N = normalize(v_normal);
    vec3 V = normalize(u_pulse_center - v_world_position);
    float edge_factor = dot(N, V);          // iluminación tipo Lambert simple
    edge_factor = clamp(edge_factor, 0.0, 1.0);
    float edge_dark = 0.7 + 0.3 * edge_factor; // ajusta 0.3-0.7 para fuerza del borde
    final_color.rgb *= edge_dark;

    float mix_ratio = 0.0;
    float check, dist;

    if(u_pulse_active == 1){
        vec3 adjusted_position = world_position - u_pulse_center;
        dist = sdSphere(adjusted_position, u_pulse_radius);

        check = when_lt(dist, 0.0) * when_gt(dist, -u_pulse_width);
        float percentage = abs(dist) / abs(u_pulse_width);
        mix_ratio = clamp(1.0 * check - percentage, 0.0, 1.0);
    }

    if(u_pulse_active == 1){
        vec4 dark_color = final_color * 0.05;
        float light_strength = clamp((-dist) * 0.5, 0.0, 1.0);
        light_strength = smoothstep(0.0, 1.0, light_strength);

        if(dist < 0.0){
            vec3 warm_tint = vec3(1.25, 1.10, 0.75);
            vec4 light_color = mix(final_color, vec4(u_pulse_color, 1.0), mix_ratio);
            light_color.rgb *= warm_tint;
            gl_FragColor = mix(dark_color, light_color, light_strength);
        }
        else {
            gl_FragColor = dark_color;
        }
    }
    else {
        gl_FragColor = final_color;
    }
}