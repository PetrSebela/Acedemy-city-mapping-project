#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "vertex.hpp"
#include "vao.hpp"

class Mesh
{
private:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    VAO buffers;

public:
    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
    Mesh();
    ~Mesh();
    void Draw();
    void Bind();
    void Unbind();
};

#endif