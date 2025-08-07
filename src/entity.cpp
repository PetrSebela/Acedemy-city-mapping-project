#include "entity.hpp"
#include <iostream>

void Entity::UpdateTRSMatrix()
{
    matrix = glm::identity<glm::mat4>();
    matrix = glm::scale(matrix, scale);
    matrix = glm::mat4(rotation) * matrix;
    matrix = glm::translate(matrix, position);
}

Entity::Entity()
{
    SetTRS(glm::vec3(0), glm::quat(), glm::vec3(1));
}

Entity::Entity(Entity *parent)
{
    SetParent(parent);
    SetTRS(glm::vec3(1), glm::quat(), glm::vec3(1));
}

Entity::~Entity()
{

}

void Entity::SetParent(Entity *parent)
{
    this->parent = parent;
}

void Entity::SetTRS(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
{
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    UpdateTRSMatrix();
}

void Entity::SetPosition(glm::vec3 position)
{
    this->position = position;
    UpdateTRSMatrix();
}

void Entity::SetRotation(glm::quat rotation)
{
    this->rotation = rotation;
    UpdateTRSMatrix();
}

void Entity::SetScale(glm::vec3 scale)
{
    this->scale = scale;
    UpdateTRSMatrix();
}

glm::vec3 Entity::GetPosition()
{
    return this->position;
}

glm::quat Entity::GetRotation()
{
    return this->rotation;
}

glm::vec3 Entity::GetScale()
{
    return this->scale;
}

glm::mat4 Entity::GetTransformMatrix()
{
    return this->matrix;
}

void Entity::AddChild(Entity child)
{
    children.push_back(child);
}

std::vector<Entity> Entity::GetChildren()
{
    return this->children;
}
