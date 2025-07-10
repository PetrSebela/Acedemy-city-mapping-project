#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>

class Texture
{
private:
    /* data */
public:
    Texture(std::string texture_path);
    Texture();
    ~Texture();
    int width, height, nrChannels;
    unsigned char *data;
};

#endif