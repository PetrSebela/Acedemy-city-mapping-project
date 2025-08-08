#version 330 core

in vec3 v_normal;
in vec2 v_uv;

out vec3 color;

uniform vec3 light_direction;

uniform sampler2D albedo;

void main(){
    // color = texture(tex_sampler, UV).rgb;
    // color = vec3(UV,1);
    // color = vec3(1);
    // color = v_normal;
    float intensity =(  0.05 + clamp(dot(v_normal, light_direction), 0, 1) * 0.85);

    // color = vec3(1,1,1) * intensity;
    color = vec3(texture(albedo, v_uv));
}