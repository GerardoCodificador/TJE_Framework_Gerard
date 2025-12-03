

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
uniform float u_specular_strength; // coeficiente de reflexión (0.0 - 1.0)
uniform float u_shininess;         // dureza del brillo (ej: 8 - 128)
// ---------------------------
// VARYINGS
// ---------------------------
varying vec2 v_uv;
varying vec3 v_world_position;
varying vec4 v_color; 
varying vec3 v_normal;          
void main() {
    vec4 final_color = u_color;

    if(u_is_texture == 0){
        final_color = texture(u_texture, v_uv);
    }

    // -------- PHONG ---------


    vec3 N = normalize(v_normal);

    // Vector luz
    vec3 light_vec = u_pulse_center - v_world_position;
    float distance = length(light_vec);
    vec3 L = normalize(light_vec);

    // View ficticio desde la luz (ya que no hay cámara)
    vec3 V = normalize(-light_vec);

    // Reflexión
    vec3 R = reflect(-L, N);

    // -------- ATENUACIÓN POR RADIO DEL PULSO --------
    // Se desvanece suavemente al llegar a u_pulse_radius
    float attenuation = 1.0 - smoothstep(
        u_pulse_radius * 0.7,
        u_pulse_radius,
        distance
    );

    // -------- COMPONENTES PHONG --------
    float ambient_strength  = 0.12;
    float diffuse_strength  = max(dot(N, L), 0.0);
   float specular_strength = pow(max(dot(R, V), 0.0), u_shininess)
                          * u_specular_strength;

    // -------- APLICAR COLOR DEL PULSO --------
    vec3 ambient  = ambient_strength * final_color.rgb;
    vec3 diffuse  = diffuse_strength * final_color.rgb * u_pulse_color;
    vec3 specular = specular_strength * u_pulse_color;

    // Aplicar atenuación
    diffuse  *= attenuation;
    specular *= attenuation;

    // Resultado final
    final_color.rgb = ambient + diffuse + specular;
    gl_FragColor = final_color;
}