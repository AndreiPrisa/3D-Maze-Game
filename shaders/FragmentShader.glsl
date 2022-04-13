#version 330

// Input
in vec3 color;

in vec3 world_pos;
in vec3 N;

uniform mat4 Model;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
// Output
layout(location = 0) out vec4 out_color;


void main()
{

    vec3 L = normalize(light_position - world_pos);
    vec3 V = normalize(eye_position - world_pos);

    float light_intensity = 5.0f;
    float ambient_light = material_kd * 0.5f;

    float diffuse_light = material_kd * light_intensity * max (dot(N,L), 0);
    vec3 R = reflect (-L, N);
    vec3 H = normalize(L + V);
    float specular_light = 0;

    if (diffuse_light > 0)
    {
        //specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
        specular_light = material_ks * light_intensity * pow(max(dot(N, H), 0), material_shininess);
    }

    float d = distance(world_pos, light_position);
    float kc = 5f;
    float kl = 1.2f;
    float kq = 2;
    float attenuation_factor = 1 / (pow(d, 2) * kq + d * kl + kc);
    out_color = vec4(color * (ambient_light +  attenuation_factor * ( diffuse_light  + specular_light )), 1);

}
