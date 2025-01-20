#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"


class ShadowMap {
public:
    ShadowMap(int width, int height);
    void Init();
    void bindFramebuffer();
    inline GLuint getFramebuffer() { return m_shadowFramebuffer; };
    inline GLuint getDepthMap() { return m_depthMap; };
    inline int getWidth() { return m_width; };
    inline int getHeight() { return m_height; };
private:
    GLuint m_shadowFramebuffer;
    GLuint m_depthMap;
    int m_width;
    int m_height;
};