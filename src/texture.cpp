#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendored/stb_image.h"

Texture::Texture(std::string texture_path)
{
    data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);

    glGenTextures(1, &this->ID);
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture()
{

}

Texture::~Texture()
{

}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->ID);
}