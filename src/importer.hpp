#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include "model.hpp"
#include <string>
#include <filesystem>


class Importer
{
private:
    /* data */
public:
    Importer(/* args */);
    ~Importer();

    static Model LoadModel(std::string raw_path);
    static Model LoadModelObj(std::string raw_path);
};



#endif