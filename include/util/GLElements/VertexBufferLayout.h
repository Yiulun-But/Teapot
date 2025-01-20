#ifndef VBLAYOUT_H
#define VBLAYOUT_H

#include <vector>
#include <type_traits>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:         return 4;
            case GL_UNSIGNED_INT:  return 4;
            case GL_UNSIGNED_BYTE: return 1;
        }
        return 0;
    }
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout() : m_Stride(0) {};

    template<typename T>
    void Push(unsigned int count){
        static_assert(std::is_same_v<T, float> || 
                  std::is_same_v<T, unsigned int> || 
                  std::is_same_v<T, unsigned char>, 
                  "Unsupported type for VertexBufferLayout::Push");
    }

    inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; };
    inline const unsigned int GetStride() const { return m_Stride; };
};

template<> void VertexBufferLayout::Push<float>(unsigned int count);
template<> void VertexBufferLayout::Push<unsigned int>(unsigned int count);
template<> void VertexBufferLayout::Push<unsigned char>(unsigned int count);

#endif