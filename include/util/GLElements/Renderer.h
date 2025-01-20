#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VertexArray.h"
#include "GLBuffers/IndexBuffer.h"
#include "GLShader.h"

class Renderer {
public:
    void IndexDraw(const VertexArray& va, const IndexBuffer& ib, const GLShader& shader);
    void ArrayDraw(const VertexArray& va, const GLShader& shader, const unsigned int count);
};


#endif