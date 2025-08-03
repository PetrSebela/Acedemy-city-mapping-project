#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;
};

#endif