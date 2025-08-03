#include "vbo.hpp"

VBO::VBO(std::vector<Vertex>& vertices)
{
    glGenBuffers(1, &this->ID);
    glBindBuffer(GL_ARRAY_BUFFER, this->ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}
VBO::~VBO()
{

}

void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->ID);
}

// Unbinds the VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &this->ID);
}