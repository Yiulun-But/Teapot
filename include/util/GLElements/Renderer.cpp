#include "Renderer.h"

void Renderer::IndexDraw(const VertexArray &va, const IndexBuffer &ib, const GLShader &shader)
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::ArrayDraw(const VertexArray &va, const GLShader &shader, const unsigned int count)
{
    shader.Bind();
    va.Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);  
}
