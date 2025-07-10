#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "glad/glad.h"
#include "model.hpp"
#include "entity.hpp"
#include <glm/gtc/quaternion.hpp>

class Renderer
{
private:

    /**
     * Enables reporting of opengl errors
     * Taken from https://github.com/meemknight/SDL2OpenglCMakeSetup/tree/master
     */
    void enableReportGlErrors();

    GLuint program_id;
    GLuint mvp_matrix_id;
    GLuint texture_id;

    GLuint vertex_buffer;
    GLuint normal_buffer;
    GLuint uv_buffer;

    glm::vec3 camera_position;
    glm::quat camera_rotation;

public:
    Renderer();
    ~Renderer();

    /**
     * Renders model
     */
    void Render(Entity entity);

    /**
     * Clears pixel buffer
     */
    void Clear();


    void UpdateCameraTransform(glm::mat4 transform);

    float fov = 90;
    
    glm::mat4 camera_transform;

    void UseWireframe(bool status);

    void UpdateCamera(glm::vec3 delta, glm::vec3 angle);
};

#endif