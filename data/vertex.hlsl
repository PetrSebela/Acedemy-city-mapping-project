#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 MVP;
uniform mat4 model_transform;

out vec2 UV;
out vec3 normal;

void main(){
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    UV = vertexUV;
    normal = (model_transform * vec4(vertex_normal,0)).xyz;
}