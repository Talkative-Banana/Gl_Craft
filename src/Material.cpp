#include "Material.h"

Material::Material(unsigned int id) : data(nullptr){
    data = texturedata[id];
}

const std::string* Material::GetString() const{
    return data;
}