#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <glad/glad.h>

class Texture
{
private:
    GLuint ID = 0;

public:
    Texture(std::string texture_path);
    Texture();
    ~Texture();
    void Bind();

    int width, height, nrChannels;
    unsigned char *data;
};

#endif