#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "GLBuffers/VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray {
private:
    unsigned int m_RendererID;
    unsigned int m_BufferCount;
public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

    void Bind() const;
    void Unbind() const;
};

#endif