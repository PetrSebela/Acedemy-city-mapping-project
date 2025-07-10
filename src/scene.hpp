#ifndef _SCENE_H_
#define _SCENE_H_

#include "entity.hpp"

class Scene
{
private:
    Entity root;

public:
    Scene(/* args */);
    ~Scene();
};

#endif