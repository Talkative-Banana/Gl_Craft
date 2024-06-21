#include "TextureCubeMap.h"
#include <iostream>
#include <../stb/stb_image.h>
TextureCubeMap::TextureCubeMap(const std::string* path) : m_RendererID(0), m_FilePath(path), m_LocalBuffer{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, m_Width(0), m_Height(0), m_BPP(0){
    // stbi_set_flip_vertically_on_load(1);

    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

    for(int i = 0; i < 6; i++) {
        unsigned char *data = stbi_load(path[i].c_str(), &m_Width, &m_Height, &m_BPP, 4);
        if(data){
            m_LocalBuffer[i] = data;
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer[i]));
            stbi_image_free(data);
        } else{
            std::cout << "Cubemap tex failed to load at path: " << path[i] << std::endl;
            stbi_image_free(data);
        }
    }

    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

TextureCubeMap::~TextureCubeMap(){
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void TextureCubeMap::Bind(unsigned int slot) const{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));
}

void TextureCubeMap::Unbind() const{
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}