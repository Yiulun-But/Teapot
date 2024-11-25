#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <cy/cyTriMesh.h>
#include <cy/cyVector.h>
#include "util/vertexGenerator.h"
#include "MatrixGenerator.h"

const GLFWvidmode* mode;

static bool load_shaders(std::string& vs, std::string& fs);
static void add_shader(GLuint shader_program, const char *shader_text, GLenum shader_type);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double x, double y);

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        std::cout << "Invalid arguement number" << std::endl;
        return 1;
    }

    std::ostringstream objOStream;

    cy::TriMesh trimesh;
    if (!trimesh.LoadFromFileObj(argv[1], false, &objOStream)) {
        std::cout << "Failed to load obj file." << std::endl;
        return 1;
    }

    void* bufferdata = (void*)objOStream.str().c_str();

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        return -1;
    }

    mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Fullscreen Window", monitor, nullptr);
    if (!window) {
        // Window or OpenGL context creation failed
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, mode->width, mode->height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    // Camera
    Camera m_Camera(mode->width, mode->height, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glfwSetWindowUserPointer(window, &m_Camera);

    // Matrix Generator
    MatrixGenerator matrixGen(mode->width, mode->height, &m_Camera);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::vector<cy::Vec3f> vBuffer;
    std::vector<cy::Vec3f> tBuffer;
    std::vector<cy::Vec3f> nBuffer;
    vertexGenerator vGen;
    vGen.generateBuffers(trimesh, vBuffer, tBuffer, nBuffer);

    GLuint vbo, tbo, nbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * vBuffer.size(), vBuffer.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (!tBuffer.empty()) {
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, tBuffer.size() * sizeof(cy::Vec3f), tBuffer.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (!nBuffer.empty()) {
        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, nBuffer.size() * sizeof(cy::Vec3f), nBuffer.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    std::string vs, fs;
    if (!load_shaders(vs, fs)) {
        std::cerr << "Failed to load shader files" << std::endl;
        return -1;
    }
    const char* vss = vs.c_str();
    const char* fss = fs.c_str();

    GLuint program = glCreateProgram();
    add_shader(program, vss, GL_VERTEX_SHADER);
    add_shader(program, fss, GL_FRAGMENT_SHADER);

    GLint success;
    GLchar ErrorLog[1024] = { 0 };
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }
    GLint mvp_location = glGetUniformLocation(program, "mvp");
    if (mvp_location == -1) {
        fprintf(stderr, "Error getting uniform location of 'mvp'.\n");
        exit(1);
    }
    GLint mn_location = glGetUniformLocation(program, "mn");
    if (mn_location == -1) {
        fprintf(stderr, "Error getting uniform location of 'mn'.\n");
        exit(1);
    }
    GLint mv_location = glGetUniformLocation(program, "mv");
    if (mv_location == -1) {
        fprintf(stderr, "Error getting uniform location of 'mv'.\n");
        exit(1);
    }
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Key and Mouse callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        matrixGen.create_matrix(trimesh);
        glm::mat4 mvp = matrixGen.MVP();
        glm::mat3 mn = matrixGen.MN();
        glm::mat4 mv = matrixGen.MV();

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix3fv(mn_location, 1, GL_FALSE, glm::value_ptr(mn));
        glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv));
        m_Camera.OnRender();
        glDrawArrays(GL_TRIANGLES, 0, vBuffer.size());

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool load_shaders(std::string &vs, std::string &fs)
{
    std::string vspath = "shader.vs";
    std::ifstream vertex_shader(vspath);

    std::string fspath = "shader.fs";
    std::ifstream fragment_shader(fspath);

    if (!vertex_shader || !fragment_shader) {
        return false;
    }

    std::ostringstream vertex_buffer;
    vertex_buffer << vertex_shader.rdbuf();
    vs = vertex_buffer.str();

    std::ostringstream fragment_buffer;
    fragment_buffer << fragment_shader.rdbuf();
    fs = fragment_buffer.str();

    return true;
}

void add_shader(GLuint shader_program, const char *shader_text, GLenum shader_type)
{
    GLuint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        std::cerr << "Shader allocation failed" << std::endl;
        exit(1);
    }
    glShaderSource(shader_obj, 1, &shader_text, NULL);
    glCompileShader(shader_obj);

    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shader_obj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s' \n", shader_type, InfoLog);
        exit(1);
    }
    glAttachShader(shader_program, shader_obj);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        Camera* m_Camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (m_Camera) {
            m_Camera->OnKeyboard(key);
        }
    }
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwTerminate();
            exit(1);
        }
    }
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        Camera* m_Camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (m_Camera) {
            m_Camera->OnMouse(x, y);
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        Camera* m_Camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (m_Camera) {
            m_Camera->OnDrag(x, y);
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        Camera* m_Camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (m_Camera) {
            m_Camera->OnRelease(x, y);
        }
    }
}
