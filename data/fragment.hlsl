#version 330 core

in vec3 v_normal;

out vec3 color;

uniform vec3 light_direction;

void main(){
    // color = texture(tex_sampler, UV).rgb;
    // color = vec3(UV,1);
    // color = vec3(1);
    // color = v_normal;

    color = vec3(1,1,1) * (  0.05 + clamp(dot(v_normal, light_direction), 0, 1) * 0.85);
}