#include "renderer.hpp"
#include "utils.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Renderer::Renderer()
{
    enableReportGlErrors();
    
    // Enable opengl features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Load shader program
    program_id = LoadShaders("../data/vertex.hlsl", "../data/fragment.hlsl");
    glUseProgram(program_id);

    mvp_matrix_id = glGetUniformLocation(program_id, "MVP");


    glm::vec3 light_position(1, 1, -1);
    GLuint light_uniform_id = glGetUniformLocation(program_id, "light_direction");
    glUniform3fv(light_uniform_id, 1, &light_position[0]);


    GLuint vert_array_id;
    glGenVertexArrays(1, &vert_array_id);
    glBindVertexArray(vert_array_id);
    glGenBuffers(1, &vertex_buffer);

    GLuint uv_array_id;
    glGenVertexArrays(1, &uv_array_id);
    glBindVertexArray(uv_array_id);
    glGenBuffers(1, &uv_buffer);

    GLuint normal_array_id;
    glGenVertexArrays(1, &normal_array_id);
    glBindVertexArray(normal_array_id);
    glGenBuffers(1, &normal_buffer);
    
    camera_transform = glm::lookAt(
        // glm::vec3(4,3,3), 
        glm::vec3(2,2,2), 
        glm::vec3(0,0,0),
        glm::vec3(0,1,0)
    );

    glGenTextures(1, &texture_id);

    camera_position = glm::vec3(0, 0, -3);
    camera_rotation = glm::identity<glm::quat>();

    camera_transform = glm::identity<glm::mat4>();
    camera_transform = glm::mat4(camera_rotation) * camera_transform;
    camera_transform = glm::translate(camera_transform, camera_position);

    printf("Renderer initialized!\n");
}

Renderer::~Renderer()
{

}

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Render(Entity entity)
{
    // Binds data
    auto vertex_size = entity.tmp_model.vertices.size() * sizeof(glm::vec3);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, entity.tmp_model.vertices.data(), GL_STATIC_DRAW);
    
    auto uv_size = entity.tmp_model.uvs.size() * sizeof(glm::vec2);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, uv_size, entity.tmp_model.uvs.data(), GL_STATIC_DRAW);

    auto normal_size = entity.tmp_model.normals.size() * sizeof(glm::vec3);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normal_size, entity.tmp_model.normals.data(), GL_STATIC_DRAW);

    // Bind texture
    // glBindTexture(GL_TEXTURE_2D, texture_id);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, model.texture.width, model.texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, model.texture.data);
    // glGenerateMipmap(GL_TEXTURE_2D);

    // Update mvp
    glm::mat4 model_matrix = entity.GetTransformMatrix();
    glm::mat4 projection_matrix = glm::perspective(glm::radians(fov / 2.0), 16.0 / 9.0, 0.1, 100.0);
    glm::mat4 mvp = projection_matrix * camera_transform * model_matrix;
    
    // Sends mvp to shader
    glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, &mvp[0][0]);

    GLuint m_uniform_id = glGetUniformLocation(program_id, "model_transform");
    glUniformMatrix4fv(m_uniform_id, 1, GL_FALSE, &model_matrix[0][0]);

    // Send buffers to shaders
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glVertexAttribPointer(
        2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Draw call
    glDrawArrays(GL_TRIANGLES, 0, entity.tmp_model.vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
}

void Renderer::UseWireframe(bool status)
{
    if(status)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


void Renderer::UpdateCamera(glm::vec3 delta, glm::vec3 angle)
{   
    // This function is cursed, none of these calculation should not work
    camera_position += glm::quat(angle) * delta;

    glm::vec3 direction;
    float yaw = -angle.y;
    float pitch = -angle.x;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    glm::quat rot = glm::angleAxis(pitch, direction) * glm::angleAxis(yaw, glm::vec3(0,1,0));
    camera_transform = glm::mat4(rot);
    camera_transform = glm::translate(camera_transform, camera_position);
}

void GLAPIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204
		|| id == 131222
		) return;
	if (type == GL_DEBUG_TYPE_PERFORMANCE) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;

}

void Renderer::enableReportGlErrors()
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}