#include "primitive.hpp"
#include <iostream>
#include "ebo.hpp"

Primitive::Primitive(Material material, std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
    this->material = material;
    this->vertices = vertices;
    this->indices = indices;

    if(vertices.size() == 0)
        printf("Warning, creating mesh with no vertices\n");

    if(indices.size() == 0)
        printf("Warning, creating mesh with no vertices\n");


    buffers.Bind();
    VBO vertex_buffer(vertices);
    EBO element_buffer(indices);

    buffers.Link(vertex_buffer, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    buffers.Link(vertex_buffer, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    buffers.Link(vertex_buffer, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
    buffers.Link(vertex_buffer, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

    buffers.Unbind();
    vertex_buffer.Unbind();
    element_buffer.Unbind();
}

Primitive::~Primitive()
{

}

void Primitive::Draw()
{

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    Unbind();
}

GLuint Primitive::Bind()
{
    buffers.Bind();
    return material.Bind();
}

void Primitive::Unbind()
{
    buffers.Unbind();
}
