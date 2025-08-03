#ifndef _EBO_H_
#define _EBO_H_

#include <vector>
#include <glad/glad.h>

class EBO
{
public:
    GLuint ID;
    EBO(std::vector<GLuint>& indices);
    ~EBO();
    void Bind();
    void Unbind();
    void Delete();
};

#endif