#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "primitive.hpp"

class Mesh
{
private:

public:
    std::vector<Primitive> primitives;
    
    Mesh();
    ~Mesh();
    void Draw();
};

#endif