#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "component.hpp"
#include "model.hpp"
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include "mesh.hpp"

class Entity
{
private:
    std::vector<Entity> children;   
    Entity* parent;

    // Transform
    glm::mat4 matrix;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    
    /**
     * Constructs TRS matrix from `position`, `rotation` and `scale` atributes
     */
    void UpdateTRSMatrix();
    
public:
    Mesh mesh;

    Entity();

    Entity(Entity* parent);
    
    
    ~Entity();

    void SetParent(Entity* parent);

    /**
     * Set entity TRS
     */
    void SetTRS(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

    /**
     * Set entity position in local space
     */
    void SetPosition(glm::vec3 position);
    
    /**
     * Set entity rotation
     */
    void SetRotation(glm::quat rotation);
    
    /**
     * Set entity scale
     */
    void SetScale(glm::vec3 scale);

    glm::vec3 GetPosition();
    
    glm::quat GetRotation();
    
    glm::vec3 GetScale();

    glm::mat4 GetTransformMatrix();
    
    void AddChild(Entity child);
};

#endif