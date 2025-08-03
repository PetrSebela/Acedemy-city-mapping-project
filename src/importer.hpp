#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include "model.hpp"
#include "entity.hpp"
#include <string>
#include <filesystem>

#include <nlohmann/json.hpp>

class Importer
{
private:
    /* data */
public:
    Importer(/* args */);
    ~Importer();

    static Entity LoadModel(std::string raw_path);
    static Entity LoadModelObj(std::string raw_path);
    static Entity LoadModelGLTF(std::string raw_path);
};



#endif