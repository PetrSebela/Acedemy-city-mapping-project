#ifndef _VBO_H_
#define _VBO_H_

#include <glad/glad.h>
#include <vector>
#include "vertex.hpp"

class VBO
{
public:
    GLuint ID;
    VBO(std::vector<Vertex>& vertices);
    ~VBO();
    void Bind();
    void Unbind();
    void Delete();
};

#endif