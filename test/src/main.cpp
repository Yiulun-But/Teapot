#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TEST/math_3d.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cy/cyTriMesh.h>
#include <cy/cyVector.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

const GLFWvidmode* mode;


static bool load_shaders(std::string& vs, std::string& fs);
static void add_shader(GLuint shader_program, const char *shader_text, GLenum shader_type);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double x, double y);



// test exclusive
glm::mat4 create_matrix(GLFWwindow* window);
glm::mat4 create_perspective(float angle, int windowWidth, int windowHeight, float zNear, float zFar);
glm::mat4 create_view(Camera& m_Camera);

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        std::cerr << "Invalid arguement number" << std::endl;
        return -1;
    }

    std::ostringstream objOStream;

    cy::TriMesh trimesh;
    if (!trimesh.LoadFromFileObj(argv[1], false, &objOStream)) {
        std::cerr << "Failed to load obj file." << std::endl;
        return -1;
    }

    void* bufferdata = (void*)objOStream.str().c_str();

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
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

    // gl faces
    glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    Vertex Vertices[8];
    Vertices[0] = Vertex(0.5f, 0.5f, 0.5f);   // Front-top-right
    Vertices[1] = Vertex(-0.5f, 0.5f, 0.5f);  // Front-top-left
    Vertices[2] = Vertex(-0.5f, -0.5f, 0.5f); // Front-bottom-left
    Vertices[3] = Vertex(0.5f, -0.5f, 0.5f);  // Front-bottom-right
    Vertices[4] = Vertex(0.5f, 0.5f, -0.5f);  // Back-top-right
    Vertices[5] = Vertex(-0.5f, 0.5f, -0.5f); // Back-top-left
    Vertices[6] = Vertex(-0.5f, -0.5f, -0.5f);// Back-bottom-left
    Vertices[7] = Vertex(0.5f, -0.5f, -0.5f); // Back-bottom-right

    unsigned int Indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 7, 6,
        6, 5, 4,

        // Left face
        1, 5, 6,
        6, 2, 1,

        // Right face
        0, 3, 7,
        7, 4, 0,

        // Top face
        0, 4, 5,
        5, 1, 0,

        // Bottom face
        3, 2, 6,
        6, 7, 3
    };

    // Camera
    Camera m_Camera(mode->width, mode->height, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glfwSetWindowUserPointer(window, &m_Camera);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

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
    GLint glScaleLocation = glGetUniformLocation(program, "gTranslation");
    if (glScaleLocation == -1) {
        fprintf(stderr, "Error getting uniform location of 'gTranslation'.\n");
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

    GLint mvp_location, vpos_location, vcol_location;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);


    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 customMatrix = create_matrix(window);

        glUniformMatrix4fv(glScaleLocation, 1, GL_FALSE, glm::value_ptr(customMatrix));

        m_Camera.OnRender();

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

glm::mat4 create_matrix(GLFWwindow* window)
{
    static float radian = 0.0f;
    static float delta = 0.001f;

    radian += delta;

    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radian, glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::mat4 rotationMatrix = glm::mat4(1.0f);

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f));

    Camera* m_Camera = (Camera*)glfwGetWindowUserPointer(window);

    glm::mat4 viewMatrix = create_view(*m_Camera);


    float FOV = 90.0f;
    glm::mat4 projectionMatrix = create_perspective(FOV, mode->width, mode->height, 1.0f, 10.0f);

    glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    glm::mat4 result = projectionMatrix * viewMatrix * modelMatrix;

    return result;
}

glm::mat4 create_perspective(float angle, int windowWidth, int windowHeight, float zNear, float zFar)
{
    float aspectRatio = (float)windowWidth / (float)windowHeight;
    glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(angle), aspectRatio, zNear, zFar);
    
    return perspectiveMatrix;
}

glm::mat4 create_view(Camera& m_Camera)
{
    // glm::mat4 viewMatrix = glm::lookAt(m_Camera.GetPos(), m_Camera.GetTarget(), m_Camera.GetUp());
    glm::vec3 forwardVec = m_Camera.GetTarget();
    glm::vec3 upVec = m_Camera.GetUp();
    glm::vec3 rightVec = glm::cross(forwardVec, upVec);

    glm::mat4 viewMatrix1 = glm::mat4(
    rightVec.x, upVec.x, -forwardVec.x, 0.0f,         
    rightVec.y, upVec.y, -forwardVec.y, 0.0f,        
    rightVec.z, upVec.z, -forwardVec.z, 0.0f,     
    0.0f, 0.0f, 0.0f, 1.0f                                   
    );
    glm::mat4 viewMatrix2 = glm::mat4(
    1.0f, 0.0f, 0.0f, 0.0f,         
    0.0f, 1.0f, 0.0f, 0.0f,        
    0.0f, 0.0f, 1.0f, 0.0f,     
    -m_Camera.GetPos().x, 
    -m_Camera.GetPos().y, 
    -m_Camera.GetPos().z, 
    1.0f                                   
    );
    return viewMatrix2 * viewMatrix1;
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

void mouse_callback(GLFWwindow *window, double x, double y)
{
    Camera* m_Camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (m_Camera) {
        m_Camera->OnMouse(x, y);
    }
}
