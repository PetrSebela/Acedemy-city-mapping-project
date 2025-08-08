#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "shader.hpp"
#include "texture.hpp"

class Material
{
private:
    Shader shader;
    Texture albedo;

public:
    Material(Shader shader, Texture albedo);
    Material();
    ~Material();

    /**
     * @return shader program ID
     */
    GLuint Bind();
    void Unbind();
};

#endif