#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

class VertexBuffer {
private:
    unsigned int m_RendererID;
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;
    void UnBind() const;

    inline int GetRendererID() const { return m_RendererID; };
};

#endif