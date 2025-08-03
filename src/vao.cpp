#include "vao.hpp"

VAO::VAO()
{
    glGenVertexArrays(1, &this->ID);
}

VAO::~VAO()
{
}

void VAO::Link(VBO &vbo, GLuint layout, GLuint count, GLenum type, GLsizeiptr stride, void *offset)
{
    vbo.Bind();
    glVertexAttribPointer(layout, count, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    vbo.Unbind();
}

void VAO::Bind()
{
    glBindVertexArray(this->ID);
}

void VAO::Unbind()
{
    glBindVertexArray(0);
}

void VAO::Delete()
{
    glDeleteVertexArrays(1, &ID);
}
