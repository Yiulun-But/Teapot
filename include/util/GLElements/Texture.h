#ifndef TEXTURE_H
#define TEXTURE_H

#include "Renderer.h"

class Texture {
private:
    unsigned int m_RendererID;
    std::string m_Filepath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline int GetWidth() const { return m_Width; };
    inline int GetHeight() const { return m_Height; };
};

#endif