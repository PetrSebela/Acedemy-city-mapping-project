#include "mesh.hpp"
#include "ebo.hpp"

#include <iostream>

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::Draw()
{
    for (Primitive p : this->primitives)
    {
        p.Draw();
    }
}