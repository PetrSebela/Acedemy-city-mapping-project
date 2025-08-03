#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 uv;

uniform mat4 MVP;
uniform mat4 model_transform;

out vec3 v_normal;

void main(){
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    // normal = (model_transform * vec4(normal, 0)).xyz;
    v_normal = normal;
    // UV = uv;
}