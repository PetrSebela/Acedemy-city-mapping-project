#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include <vector>
#include "vao.hpp"
class Primitive
{
private:
    VAO buffers;

public:
Primitive(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
~Primitive();
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    void Draw();
    void Bind();
    void Unbind();
};

#endif