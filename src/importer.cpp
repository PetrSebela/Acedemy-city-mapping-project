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

    if(extension == ".gltf")
    {
        return LoadModelGLTF(raw_path);
    }
    
    printf("Unknown file type\n");
    Entity e;
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

template <typename T> std::vector<T> GetVectors(std::vector<std::vector<unsigned char>> buffers, json gltf, int accesor_index)
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

    int stride = sizeof(T);
    if(bufferview.contains("byteStride"))
    {
        stride = bufferview["byteStride"];
    }
        
    std::vector<T> vectors;
    for (int i = 0; i < length; i++)
    {        
        auto x = *(T*)(buffer_target + i * stride);
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

std::vector<Mesh> GetMeshes(json gltf, std::string path, std::vector<Material> materials)
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
            auto positions = GetVectors<glm::vec3>(buffers, gltf, attributes["POSITION"]);
            auto normals = GetVectors<glm::vec3>(buffers, gltf, attributes["NORMAL"]);
            auto uvs = GetVectors<glm::vec2>(buffers, gltf, attributes["TEXCOORD_0"]);

            std::vector<Vertex> vertices(positions.size());

            for (size_t i = 0; i < vertices.size(); i++)
            {
                vertices[i].position = positions[i];
                vertices[i].normal = normals[i];
                vertices[i].uv = uvs[i];
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

            int material_index = mesh_descriptor["material"];

            Primitive primitive(materials[material_index], vertices, indices);
            mesh.primitives.push_back(primitive);
        }

        meshes.push_back(mesh);
    }
    
    
    return meshes;
}

std::vector<Material> GetMaterial(json gltf, path directory)
{
    std::vector<Material> materials;

    Shader shader("../data/vertex.hlsl", "../data/fragment.hlsl");

    json textures = gltf["textures"];
    json images = gltf["images"];

    
    for (size_t i = 0; i < gltf["materials"].size(); i++)
    {
        std::cout << "iter" << std::endl;
        json material_description = gltf["materials"][i];
        std::cout << material_description << std::endl;
        
        if(material_description.contains("extensions"))
        {
            Material material;
            materials.push_back(material);
            continue;
        }
        
        int texture_index = material_description["pbrMetallicRoughness"]["baseColorTexture"]["index"];
        int image_index = textures[texture_index]["source"];
        
        std::string relative_path = images[image_index]["uri"];
        Texture albedo(directory / relative_path);
        
        Material material(shader, albedo);
        materials.push_back(material);
    }
    
    return materials;
}

Entity Importer::LoadModelGLTF(std::string raw_path)
{    
    std::ifstream input(raw_path);
    json gltf;
    input >> gltf;

    
    std::filesystem::path directory = std::filesystem::path(raw_path).parent_path();
    auto materials = GetMaterial(gltf, directory);

    auto meshes = GetMeshes(gltf, raw_path, materials);
    printf("Total mesh count %li\n", meshes.size());
    
    int scene_index = gltf["scene"];
    json nodes = gltf["scenes"][scene_index]["nodes"];

    Entity scene;   // Root object
    LoadNodes(scene, nodes, gltf["nodes"], meshes);
    return scene;
}
