#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 object_color;
uniform float timeValue;
uniform float hitTimeValue;
uniform float health;
uniform float fullHealth;
uniform float spikeLength;

// Output value to fragment shader
out vec3 color;

out vec3 world_pos;
out vec3 N;

float spikeAnimation(vec3 normal) {
    float angle = timeValue;
    float sharpness = 25;
    mat2 rotationMatrix = transpose(mat2(cos(angle), -sin(angle), sin(angle), cos(angle)));
    vec2 first = rotationMatrix * vec2(1, 0);
    vec2 second = rotationMatrix * vec2(0, 1);
    return pow(abs(dot(vec2(normal.y, normal.z), first)), sharpness) * spikeLength +
    pow(abs(dot(vec2(normal.x, normal.z), second)), sharpness) * spikeLength
    + pow(abs(dot(vec2(normal.x, normal.y), first)), sharpness) * spikeLength;
}

float damageAnimation(vec3 p)
{
   return fract(sin(dot(p.xz ,vec2(167.9898,58.233))) * 43758.5453 ) * hitTimeValue / 2 * pow(fullHealth / (max(1, health)), 0.35);
}

void main()
{
    N = normalize(vec3 (Model * vec4(v_normal, 0)));
    float displacement;
    if (hitTimeValue == 0) {
        displacement = spikeAnimation(N);
    } else {
        displacement = damageAnimation(N);
    }
    vec3 new_position = v_position + displacement * N;
    gl_Position = Projection * View * Model * vec4(new_position, 1.0);
    if (hitTimeValue == 0) {
        color = vec3(object_color.x, object_color.y, object_color.z + 20 * displacement);
    } else {
        color = vec3(object_color.x + 2 * displacement, object_color.y, object_color.z + 15 * displacement);
    }
    world_pos = vec3 (Model * vec4(new_position, 1));
}
