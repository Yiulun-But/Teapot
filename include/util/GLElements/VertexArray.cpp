#include "VertexArray.h"
#include <iostream>

VertexArray::VertexArray() : m_BufferCount(0)
{
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::addBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
{
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i) {
        const auto& element = elements[i];
        glEnableVertexAttribArray(m_BufferCount);
        glVertexAttribPointer(m_BufferCount, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<const void*>(offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
        m_BufferCount++;
    }
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
