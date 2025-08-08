#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <glad/glad.h>

class Shader
{
private:
    GLuint ID;

public:
    Shader(std::string vertex_shader_path, std::string fragment_shader_path);
    Shader();
    ~Shader();
    GLuint Bind();
    void Unbind();
};

#endif