#include "material.hpp"

Material::Material(Shader shader, Texture albedo)
{
    this->shader = shader;
    this->albedo = albedo;
}

Material::Material()
{
    this->shader = Shader("../data/vertex.hlsl", "../data/fragment.hlsl");
}

Material::~Material()
{
}


GLuint Material::Bind()
{
    auto id = this->shader.Bind();
    this->albedo.Bind();
    return id;
}

