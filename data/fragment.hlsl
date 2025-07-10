#version 330 core
in vec2 UV;
in vec3 normal;

out vec3 color;

uniform sampler2D tex_sampler;

uniform vec3 light_direction;

void main(){
    // color = texture(tex_sampler, UV).rgb;
    color = vec3(UV,1);
    color = vec3(1,1,1) * (  0.05 + clamp(dot(normal, light_direction), 0, 1) * 0.95);
}