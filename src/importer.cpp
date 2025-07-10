#include "importer.hpp"

#include <fstream>
#include <iostream>
#include "utils.hpp"

Importer::Importer()
{
}

Importer::~Importer()
{
}

Model Importer::LoadModel(std::string raw_path)
{
    auto path = std::filesystem::path(raw_path);
    auto extension = path.extension();
    
    if(extension == ".obj")
    {
        return LoadModelObj(raw_path);
    }
    
    printf("Unknown file type\n");
    Model model;
    return model;
}

Model Importer::LoadModelObj(std::string raw_path)
{
    Model model;

    std::ifstream input(raw_path);
    std::string line;

    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> uvs;

    while (getline(input, line))
    {
        auto tokens = split(line, ' ');
        
        if(tokens[0] == "v")
        {
            glm::vec3 vertex(
                std::stof(tokens[1]),
                std::stof(tokens[2]),
                std::stof(tokens[3])           
            );

            vertices.push_back(vertex);
        }
        
        if(tokens[0] == "vn")
        {
            glm::vec3 normal(
                std::stof(tokens[1]),
                std::stof(tokens[2]),
                std::stof(tokens[3])           
            );

            normals.push_back(normal);
        }
        
        if(tokens[0] == "vt")
        {
       

            glm::vec2 uv(
                std::stof(tokens[1]),
                std::stof(tokens[2])
            );

            uvs.push_back(uv);
        }

        if(tokens[0] == "f")
        {
            auto vertA = tokens[1]; 
            auto vertB = tokens[2]; 
            auto vertC = tokens[3]; 

            auto va_tokens = split(vertA, '/');
            auto vb_tokens = split(vertB, '/');
            auto vc_tokens = split(vertC, '/');

            int vert_index_a = std::stoi(va_tokens[0]) - 1;
            int vert_index_b = std::stoi(vb_tokens[0]) - 1;
            int vert_index_c = std::stoi(vc_tokens[0]) - 1;

            if (va_tokens[1] != "" && vb_tokens[1] != "" && vc_tokens[1] != "")
            {
                int uv_index_a = std::stoi(va_tokens[1]) - 1;
                int uv_index_b = std::stoi(vb_tokens[1]) - 1;
                int uv_index_c = std::stoi(vc_tokens[1]) - 1;

                model.uvs.push_back(uvs[uv_index_a]);
                model.uvs.push_back(uvs[uv_index_b]);
                model.uvs.push_back(uvs[uv_index_c]);
            }

            int normal_index_a = std::stoi(va_tokens[2]) - 1;
            int normal_index_b = std::stoi(vb_tokens[2]) - 1;
            int normal_index_c = std::stoi(vc_tokens[2]) - 1;

            model.vertices.push_back(vertices[vert_index_a]);
            model.vertices.push_back(vertices[vert_index_b]);
            model.vertices.push_back(vertices[vert_index_c]);

            model.normals.push_back(normals[normal_index_a]);
            model.normals.push_back(normals[normal_index_b]);
            model.normals.push_back(normals[normal_index_c]);

            if(uvs.size() == 0)
            {
                model.uvs.push_back(glm::vec2(0));
                model.uvs.push_back(glm::vec2(0));
                model.uvs.push_back(glm::vec2(0));

                continue;
            }

        }
    }

    input.close();

    printf("Vertex count %ld\n", model.vertices.size());
    printf("Normal count %ld\n", model.normals.size());

    return model;
}
