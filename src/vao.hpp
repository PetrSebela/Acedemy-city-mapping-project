#ifndef _VAO_H_
#define _VAO_H_

#include <glad/glad.h>
#include "vbo.hpp"

class VAO
{
public:
    GLuint ID;
    VAO();
    ~VAO();
    void Link(VBO& vbo, GLuint layout, GLuint count, GLenum type, GLsizeiptr stride, void* offset);
    void Bind();
    void Unbind();
    void Delete();
};

#endif