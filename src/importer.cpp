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

    for (size_t i = 0; i < root["buffers"].size(); i++)
    {        
        std::filesystem::path buffer_file(root["buffers"][i]["uri"]);
        auto buffer = GetBuffer(path / buffer_file);
        buffers.push_back(buffer);
    }
    
    return buffers;
}

template <typename T> std::vector<GLuint> GetScalars(std::vector<std::vector<unsigned char>> buffers, json gltf, int accesor_index)
{
    json accesor = gltf["accessors"][accesor_index];
    int bufferview_index = accesor["bufferView"];
    json bufferview = gltf["bufferViews"][bufferview_index];

    // Get buffer offset
    int offset = 0; 
    if(bufferview.contains("byteOffset"))
    {
        offset += (int)bufferview["byteOffset"];
    }

    if(accesor.contains("byteOffset"))
    {
        offset += (int)accesor["byteOffset"];
    }


    // retrive desired buffer
    auto buffer = buffers[bufferview["buffer"]];

    int length = accesor["count"];

    auto buffer_target = (T*)(buffer.data() + offset);

    std::vector<GLuint> ints;

    for (int i = 0; i < length; i++)
    {
        ints.push_back((GLuint)buffer_target[i]);
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
        offset += (int)bufferview["byteOffset"];
    }
    
    if(accesor.contains("byteOffset"))
    {
        offset += (int)accesor["byteOffset"];
    }

    // retrive desired buffer
    auto buffer = buffers[bufferview["buffer"]];

    int length = accesor["count"];

    // auto buffer_target = (glm::vec3*)(buffer.data() + offset);
    auto buffer_target = buffer.data() + offset;

    int stride = sizeof(glm::vec3);
    if(bufferview.contains("byteStride"))
    {
        stride = bufferview["byteStride"];
    }
        
    std::vector<glm::vec3> vectors;
    for (int i = 0; i < length; i++)
    {        
        auto x = *(glm::vec3*)(buffer_target + i * stride);
        vectors.push_back(x);
    }

    return vectors;
}

void LoadNodes(Entity& parent, json children, json nodes, std::vector<Mesh> meshes)
{
    for (json::iterator it = children.begin(); it != children.end(); ++it) {
        int index = *it;
        json node = nodes[index];
        
        glm::vec3 translation(0);
        if(node.count("translation") == 1)
        {
            json t = node["translation"];
            translation = glm::vec3((float)t[0], (float)t[1], (float)t[2]);
        }
        
        // std::cout << "asd" << std::endl;
        Entity e(&parent);
        e.SetPosition(translation);
        
        if (node.count("name") != 0)
        {
            e.name = node["name"];
        }

        if(node.count("mesh") != 0)
        {
            int mesh_index = node["mesh"];
            e.mesh = meshes[mesh_index];
            std::cout << "mesh import for: " << e.name << std::endl;
        }

        LoadNodes(e, node["children"], nodes, meshes);
        parent.AddChild(e);
    }
}

std::vector<Mesh> GetMeshes(json gltf, std::string path)
{
    std::filesystem::path directory = std::filesystem::path(path).parent_path();
    auto buffers = GetBuffers(directory, gltf); // Loads raw butes from buffer
    std::cout << "Buffers loaded" << std::endl;

    std::vector<Mesh> meshes;

    for (size_t i = 0; i < gltf["meshes"].size(); i++)
    {
        Mesh mesh;

        for (size_t primitive_index = 0; primitive_index < gltf["meshes"][i]["primitives"].size(); primitive_index++)
        {
            json mesh_descriptor = gltf["meshes"][i]["primitives"][primitive_index];
            json attributes = mesh_descriptor["attributes"];

            // Get positions
            auto positions = GetVectors(buffers, gltf, attributes["POSITION"]);
            auto normals = GetVectors(buffers, gltf, attributes["NORMAL"]);

            std::vector<Vertex> vertices(positions.size());

            
            for (size_t i = 0; i < vertices.size(); i++)
            {
                vertices[i].position = positions[i];
                vertices[i].normal = normals[i];
            }
            
            // Get indices
            std::vector<GLuint> indices;
            if( gltf["accessors"][(int)mesh_descriptor["indices"]]["componentType"] == 5123)
            {
                indices = GetScalars<unsigned short>(buffers, gltf, mesh_descriptor["indices"]);
            }
            if( gltf["accessors"][(int)mesh_descriptor["indices"]]["componentType"] == 5125)
            {
                indices = GetScalars<unsigned int>(buffers, gltf, mesh_descriptor["indices"]);
            }

            Primitive primitive(vertices, indices);
            mesh.primitives.push_back(primitive);
        }

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
    printf("Total mesh count %li\n", meshes.size());
    
    int scene_index = gltf["scene"];
    json nodes = gltf["scenes"][scene_index]["nodes"];

    Entity scene;   // Root object
    LoadNodes(scene, nodes, gltf["nodes"], meshes);
    return scene;
}
