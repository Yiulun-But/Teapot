#include "GLShader.h"
#include <iostream>
#include <fstream>
#include <sstream>

GLShader::GLShader(std::string vs_loc, std::string fs_loc) : m_VSLoc(vs_loc), m_FSLoc(fs_loc)
{
    m_RendererID = glCreateProgram();
    Init();
}

GLShader::~GLShader()
{
    glDeleteProgram(m_RendererID);
}

void GLShader::Init()
{
    ShaderSource src;
    if (!load_shaders(src)) {
        std::cerr << "Failed to load shader files" << std::endl;
        glDeleteProgram(m_RendererID);
        m_RendererID = 0;
        return;
    }
    const char* vss = src.vs_source.c_str();
    const char* fss = src.fs_source.c_str();

    if (!add_shader(m_RendererID, vss, GL_VERTEX_SHADER)) return;
    if (!add_shader(m_RendererID, fss, GL_FRAGMENT_SHADER)) return;

    GLint success;
    GLchar ErrorLog[1024] = { 0 };
    glLinkProgram(m_RendererID);
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(m_RendererID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader m_RendererID: '%s'\n", ErrorLog);
        return;
    }

    glValidateProgram(m_RendererID);
    glGetProgramiv(m_RendererID, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_RendererID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader m_RendererID: '%s'\n", ErrorLog);
        return;
    }
}

void GLShader::Bind() const
{
    glUseProgram(m_RendererID);
}

void GLShader::Unbind() const
{
    glUseProgram(0);
}

void GLShader::setUniform1i(const std::string &name, int v)
{
    glUniform1i(GetUniformLocation(name), v);
}

void GLShader::setUniform1f(const std::string &name, float v)
{
    glUniform1f(GetUniformLocation(name), v);
}

void GLShader::setUniform3fv(const std::string &name, unsigned int count, const void *vector)
{
    glUniform3fv(GetUniformLocation(name), count, (float*)vector);
}

void GLShader::setUniformMatrix4fv(const std::string &name, unsigned int count, const void *matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, (float*)matrix);
}

void GLShader::setUniformMatrix3fv(const std::string &name, unsigned int count, const void *matrix)
{
    glUniformMatrix3fv(GetUniformLocation(name), count, GL_FALSE, (float*)matrix);
}

int GLShader::GetUniformLocation(const std::string &name)
{
    if (m_UniformCache.find(name) != m_UniformCache.end()) 
        return m_UniformCache[name];
    
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        fprintf(stderr, "Error getting uniform location of '%s'.\n", name);
    }

    m_UniformCache[name] = location;
    return location;
}

bool GLShader::load_shaders(ShaderSource& src)
{
    std::string vspath = m_VSLoc;
    std::ifstream vertex_shader(vspath);

    std::string fspath = m_FSLoc;
    std::ifstream fragment_shader(fspath);

    if (!vertex_shader || !fragment_shader) {
        return false;
    }

    std::ostringstream vertex_buffer;
    vertex_buffer << vertex_shader.rdbuf();
    src.vs_source = vertex_buffer.str();

    std::ostringstream fragment_buffer;
    fragment_buffer << fragment_shader.rdbuf();
    src.fs_source = fragment_buffer.str();

    return true;
}

unsigned int GLShader::add_shader(GLuint shader_program, const char *shader_text, GLenum shader_type)
{
    GLuint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        std::cerr << "Shader allocation failed" << std::endl;
        return 0;
    }
    glShaderSource(shader_obj, 1, &shader_text, NULL);
    glCompileShader(shader_obj);

    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shader_obj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s' \n", shader_type, InfoLog);
        glDeleteShader(shader_obj);
        return 0;
    }
    glAttachShader(shader_program, shader_obj);
    glDeleteShader(shader_obj);

    return 1;
}

