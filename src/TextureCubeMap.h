#pragma once

#include "Renderer.h"
#include <string>
class TextureCubeMap{
private:
    unsigned int m_RendererID;
    const std::string *m_FilePath;
    unsigned char* m_LocalBuffer[6];
    int m_Width, m_Height, m_BPP;
public:
    // type 0 normal
    // type 1 cubemap
    TextureCubeMap(const std::string* path);
    ~TextureCubeMap();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
};