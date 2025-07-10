#include "model.hpp"

Model::Model()
{
    transform = glm::mat4(1);
    // for (size_t i = 0; i < 108; i += 3)
    // {
    //     glm::vec3 vertex(
    //         cube_vertices[i],
    //         cube_vertices[i+1],
    //         cube_vertices[i+2]
    //      );
    //     vertices.push_back(vertex);
    // }

    // for (size_t i = 0; i < 72; i += 2)
    // {
    //     glm::vec2 uv(
    //         cube_uvs[i],
    //         cube_uvs[i+1]
    //     );
    //     uvs.push_back(uv);
    // }

    // // texture = Texture("../data/ptmt.bmp");
    // texture = Texture("../data/ratatuj.jpeg");
}

Model::~Model()
{
}