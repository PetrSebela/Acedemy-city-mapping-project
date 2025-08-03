#include "importer.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include "utils.hpp"

Importer::Importer()
{
}

Importer::~Importer()
{
}

Entity Importer::LoadModel(std::string raw_path)
{
    auto path = std::filesystem::path(raw_path);
    auto extension = path.extension();
    
    if(extension == ".obj")
    {
        return LoadModelObj(raw_path);
    }

    if(extension == ".gltf")
    {
        return LoadModelGLTF(raw_path);
    }
    
    printf("Unknown file type\n");
    Entity e;
    return e;
}

Entity Importer::LoadModelObj(std::string raw_path)
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


    Entity e;
    // e.tmp_model = model;
    return e;
}

using json = nlohmann::json;
using path = std::filesystem::path;


std::vector<unsigned char> GetBuffer(std::string path)
{
    std::ifstream buffer_file(path, std::ios::binary);
    std::vector<unsigned char>buffer((std::istreambuf_iterator<char>(buffer_file)), std::istreambuf_iterator<char>());
    return buffer;
}

std::vector<std::vector<unsigned char>> GetBuffers(path path, json root)
{
    std::vector<std::vector<unsigned char>> buffers;

    for (int i = 0; i < root["buffers"].size(); i++)
    {        
        std::filesystem::path buffer_file(root["buffers"][i]["uri"]);
        auto buffer = GetBuffer(path / buffer_file);
        buffers.push_back(buffer);
    }
    
    return buffers;
}

std::vector<GLuint> GetInts(std::vector<std::vector<unsigned char>> buffers, json gltf, int accesor_index)
{

    json accesor = gltf["accessors"][accesor_index];
    int bufferview_index = accesor["bufferView"];
    json bufferview = gltf["bufferViews"][bufferview_index];

    // Get buffer offset
    int offset = 0; 
    if(bufferview.contains("byteOffset"))
    {
        offset = bufferview["byteOffset"];
    }

    // retrive desired buffer
    auto buffer = buffers[bufferview["buffer"]];

    int length = bufferview["byteLength"];

    auto buffer_target = (unsigned short*)(buffer.data() + offset);

    std::vector<GLuint> ints;

    for (int i = 0; i < length; i++)
    {
        ints.push_back(buffer_target[i]);
    }

    return ints;
}

std::vector<glm::vec3> GetVectors(std::vector<std::vector<unsigned char>> buffers, json gltf, int accesor_index)
{

    json accesor = gltf["accessors"][accesor_index];
    int bufferview_index = accesor["bufferView"];
    json bufferview = gltf["bufferViews"][bufferview_index];

    // Get buffer offset
    int offset = 0; 
    if(bufferview.contains("byteOffset"))
    {
        offset = bufferview["byteOffset"];
    }

    // retrive desired buffer
    auto buffer = buffers[bufferview["buffer"]];

    int length = bufferview["byteLength"];

    auto buffer_target = (glm::vec3*)(buffer.data() + offset);

    std::vector<glm::vec3> vectors;

    for (int i = 0; i < length; i++)
    {
        vectors.push_back(buffer_target[i]);
    }

    return vectors;
}

void LoadNodes(Entity parent, json indices, json nodes)
{
    for (json::iterator it = indices.begin(); it != indices.end(); ++it) {
        int index = *it;
        json node = nodes[index];
        Entity e(parent);

        parent.AddChild(e);
        if(node.count("children") == 0)
            continue;

        LoadNodes(e, node["children"], nodes);
    }
}

std::vector<Mesh> GetMeshes(json gltf, std::string path)
{
    std::filesystem::path directory = std::filesystem::path(path).parent_path();
    auto buffers = GetBuffers(directory, gltf);

    std::vector<Mesh> meshes;

    for (int i = 0; i < gltf["meshes"].size(); i++)
    {
        json mesh_descriptor = gltf["meshes"][i]["primitives"][0];
        json attributes = mesh_descriptor["attributes"];


        
        // Get positions
        auto positions = GetVectors(buffers, gltf, attributes["POSITION"]);
        auto normals = GetVectors(buffers, gltf, attributes["NORMAL"]);

        std::vector<Vertex> vertices(positions.size());

        for (int i = 0; i < vertices.size(); i++)
        {
            vertices[i].position = positions[i];
            vertices[i].normal = normals[i];
        }

        // Get indices
        auto indices = GetInts(buffers, gltf, mesh_descriptor["indices"]);

        Mesh mesh(vertices, indices);
        meshes.push_back(mesh);
    }
    
    return meshes;
}

Entity Importer::LoadModelGLTF(std::string raw_path)
{
    
    std::ifstream input(raw_path);
    json gltf;
    input >> gltf;

    auto meshes = GetMeshes(gltf, raw_path);
    
    Entity scene;   // Root object 
    scene.mesh = meshes[0];
    return scene;
}
