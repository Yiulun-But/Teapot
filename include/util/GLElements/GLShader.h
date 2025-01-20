#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct ShaderSource {
    std::string vs_source;
    std::string fs_source;
};

class GLShader {
private:
    unsigned int m_RendererID;
    std::string m_VSLoc;
    std::string m_FSLoc;
    std::unordered_map<std::string, int> m_UniformCache;
public:
    GLShader(std::string vs_loc, std::string fs_loc);
    ~GLShader();

    void Bind() const;
    void Unbind() const;

    void setUniform1i(const std::string& name, int v);
    void setUniform1f(const std::string& name, float v);
    void setUniform3fv(const std::string& name, unsigned int count, const void* vector);
    void setUniformMatrix4fv(const std::string& name, unsigned int count, const void* matrix);
    void setUniformMatrix3fv(const std::string& name, unsigned int count, const void* matrix);



private:
    int GetUniformLocation(const std::string& name);
    bool load_shaders(ShaderSource& src);
    unsigned int add_shader(GLuint shader_program, const char *shader_text, GLenum shader_type);

    void Init();
};

#endif