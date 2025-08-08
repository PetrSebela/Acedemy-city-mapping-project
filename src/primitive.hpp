#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include <vector>
#include "vao.hpp"
#include "material.hpp"

class Primitive
{
private:
    VAO buffers;
    Material material;

public:
Primitive(Material material, std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
~Primitive();
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    void Draw();
    GLuint Bind();
    void Unbind();
};

#endif