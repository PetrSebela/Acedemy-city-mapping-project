#include "mesh.hpp"
#include "ebo.hpp"

#include <iostream>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
    this->vertices = vertices;
    this->indices = indices;

    buffers.Bind();
    VBO vertex_buffer(vertices);
    EBO element_buffer(indices);

    buffers.Link(vertex_buffer, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    buffers.Link(vertex_buffer, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));

    buffers.Unbind();
    vertex_buffer.Unbind();
    element_buffer.Unbind();
}

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::Draw()
{
    Bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    Unbind();
}

void Mesh::Bind()
{
    buffers.Bind();
}

void Mesh::Unbind()
{
    buffers.Unbind();
}