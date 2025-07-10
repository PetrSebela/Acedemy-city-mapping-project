#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendored/stb_image.h"

Texture::Texture(std::string texture_path)
{
    data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
}

Texture::Texture()
{

}

Texture::~Texture()
{

}