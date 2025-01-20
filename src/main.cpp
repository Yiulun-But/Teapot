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
#include <glm/gtx/string_cast.hpp>
#include "ShadowMap.h"
#include "util/Objects/RenderObject.h"
#include "util/Objects/SpotLight.h"
#include "util/GLElements/GLShader.h"
#include "util/GLElements/Texture.h"
#include "util/GLElements/VertexArray.h"
#include "util/GLElements/GLBuffers/VertexBuffer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double x, double y);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float angle = 0.0f;
float yoffset = 0;

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        std::cout << "Invalid arguement number" << std::endl;
        return 1;
    }

    int windowWidth = 1200;
    int windowHeight = 900;


    // Matrix Generator
    MatrixGenerator matrixGen(windowWidth, windowHeight);

    // Teapot object
    cy::TriMesh trimesh;
    std::ostringstream objOStream;
    if (!trimesh.LoadFromFileObj(argv[1], true, &objOStream)) {
        std::cout << "Failed to load obj file." << std::endl;
        return 1;
    }
    void* bufferdata = (void*)objOStream.str().c_str();

    // RenderObject Class
    RenderObject teapot(
        glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)),
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
        trimesh
        );

    teapot.loadBuffers();

    // Light object
    cy::TriMesh liTrimesh;
    std::ostringstream liObjOStream;
    if (!liTrimesh.LoadFromFileObj("assets/light.obj", true, &liObjOStream)) {
        std::cout << "Failed to load light obj file." << std::endl;
        return 1;
    }
    void* lightbufferdata = (void*)liObjOStream.str().c_str();

    // Set light object properties
    teapot.getMeshObj().ComputeBoundingBox();
    cy::Vec3f boxVolumn = (teapot.getMeshObj().GetBoundMax() - teapot.getMeshObj().GetBoundMin()).Abs();
    float light_x = 0.25f * 50.0f * glm::cos(glm::radians(angle));
    float light_z = -0.25f * 50.0f * glm::sin(glm::radians(angle));
    glm::vec3 lightPos = glm::vec3(light_x, 0.25f * boxVolumn.z, light_z);
    SpotLight spotlight(
        lightPos,
        glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25)),
        matrixGen.light_rotation_matrix(lightPos, teapot.getWorldPos()),
        glm::translate(glm::mat4(1.0f), lightPos),
        1.0f,
        glm::vec3(1.0f, 1.0, 1.0),
        teapot.getWorldPos() - lightPos,
        0.9063f,
        liTrimesh
        );

    spotlight.loadBuffers();

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Windowed", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    // Camera
    Camera m_Camera(
        windowWidth, 
        windowHeight, 
        glm::vec3(0.0f, 0.0f, 10.0f), 
        glm::vec3(0.0f, 0.0f, -1.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f));
    glfwSetWindowUserPointer(window, &m_Camera);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    vertexGenerator vGen;

    VertexBuffer teapotVBO(teapot.getVBuffer().data(), sizeof(cy::Vec3f) * teapot.getVBuffer().size());
    VertexBuffer teapotTBO(teapot.getTBuffer().data(), sizeof(cy::Vec2f) * teapot.getTBuffer().size());
    VertexBuffer teapotNBO(teapot.getNBuffer().data(), sizeof(cy::Vec3f) * teapot.getNBuffer().size());

    VertexArray teapotVAO;
    teapotVAO.Bind();
    VertexBufferLayout VBNBLayout;
    VBNBLayout.Push<float>(3);
    VertexBufferLayout TBLayout;
    TBLayout.Push<float>(2);
    teapotVAO.addBuffer(teapotVBO, VBNBLayout);
    teapotVAO.addBuffer(teapotTBO, TBLayout);
    teapotVAO.addBuffer(teapotNBO, VBNBLayout);

    // Teapot shader
    GLShader teapotShader("shaders/shader.vs", "shaders/shader.fs");

    // Set diffuse texture
    glActiveTexture(GL_TEXTURE0);
    Texture teapotDiffuseTex("assets/brick.png");
    teapotDiffuseTex.Bind(0);

    // Set specular texture
    glActiveTexture(GL_TEXTURE1);
    Texture teapotSpecularTex("assets/brick-specular.png");
    teapotDiffuseTex.Bind(1);

    // Plane setup
    Object plane(
        glm::vec3(0.0f, -0.25f * boxVolumn.z * 0.5, 0.0f), 
        glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)),
        glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.25f * boxVolumn.z * 0.5, 0.0f))
    );


    VertexArray planeVAO;
    planeVAO.Bind();
    float vertexData[] = {
        -100.0f, 0.0f, 100.0f,  
        100.0f, 0.0f, 100.0f, 
        100.0f,  0.0f, -100.0f,
        -100.0f, 0.0f, 100.0f, 
        100.0f,  0.0f, -100.0f,
        -100.0f, 0.0f, -100.0f,  
    };
    VertexBuffer planeVBO(vertexData, sizeof(vertexData));
    planeVAO.addBuffer(planeVBO, VBNBLayout);

    GLShader planeShader("shaders/plane.vs", "shaders/plane.fs");

    // Light setup
    VertexArray lightVAO;
    lightVAO.Bind();

    VertexBuffer lightVBO(spotlight.getVBuffer().data(), sizeof(cy::Vec3f) * spotlight.getVBuffer().size());
    lightVAO.addBuffer(lightVBO, VBNBLayout);

    GLShader lightShader("shaders/light.vs", "shaders/light.fs");

    // Retrieve original frame buffer
    GLint origFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFB);

    // ShadowMap Shader
    GLShader shadowShader("shaders/shadow_map.vs", "shaders/shadow_map.fs");

    // Shadowmap setup
    glActiveTexture(GL_TEXTURE2);
    ShadowMap shadowmap(2 * windowWidth, 2 * windowHeight);

    // Renderer
    Renderer SceneRenderer;

    // Light camera setup
    glm::vec3 default_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(spotlight.getDirection(), default_up));
    glm::vec3 orthogonal_up = glm::cross(right, spotlight.getDirection());
    Camera li_Camera(windowWidth, windowHeight, spotlight.getWorldPos(), spotlight.getDirection(), orthogonal_up);

    // Key and Mouse callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    // Test

    while(!glfwWindowShouldClose(window))
    {
        // Move light source
        light_x = 0.25f * 50.0f * glm::cos(glm::radians(angle));
        light_z = -0.25f * 50.0f * glm::sin(glm::radians(angle));
        lightPos = glm::vec3(light_x, 0.25f * boxVolumn.z, light_z);
        spotlight.setWorldPos(lightPos);
        spotlight.setWorldRotate(matrixGen.light_rotation_matrix(lightPos, teapot.getWorldPos()));
        spotlight.setWorldTrans(glm::translate(glm::mat4(1.0f), lightPos));
        spotlight.setDirection(teapot.getWorldPos() - lightPos);
        default_up = glm::vec3(0.0f, 1.0f, 0.0f);
        right = glm::normalize(glm::cross(spotlight.getDirection(), default_up));
        orthogonal_up = glm::cross(right, spotlight.getDirection());
        li_Camera.SetPos(spotlight.getWorldPos());
        li_Camera.SetTarget(spotlight.getDirection());
        li_Camera.SetUp(orthogonal_up);

        m_Camera.OnRender();
        
        // Render to shadow map
        shadowmap.bindFramebuffer();
        glViewport(0, 0, shadowmap.getWidth(), shadowmap.getHeight());
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 smvp = matrixGen.create_mvp(teapot, li_Camera);

        // Render Teapot
        teapotVAO.Bind();
        shadowShader.Bind();
        shadowShader.setUniformMatrix4fv("smvp", 1, glm::value_ptr(smvp));
        SceneRenderer.ArrayDraw(teapotVAO, shadowShader, teapot.getVBuffer().size());

        // Render to back buffer
        glBindFramebuffer(GL_FRAMEBUFFER, origFB); 
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp = matrixGen.create_mvp(teapot, m_Camera);
        glm::mat4 mv = matrixGen.create_mv(teapot, m_Camera);
        glm::mat3 mn = matrixGen.create_mn(mv);
        glm::mat4 v = matrixGen.create_v(m_Camera);
        glm::mat4 pm = matrixGen.create_mvp(plane, m_Camera);
        glm::mat4 vp = matrixGen.create_vp(m_Camera);
        glm::mat4 lm = matrixGen.create_mvp(spotlight, m_Camera);
        glm::mat4 pmv = matrixGen.create_mv(plane, m_Camera);


        // Render Teapot
        teapotVAO.Bind();
        teapotShader.Bind();
        teapotShader.setUniform1i("TeapotDepthMap", 2);
        teapotShader.setUniformMatrix4fv("mvp", 1, glm::value_ptr(mvp));
        teapotShader.setUniformMatrix3fv("mn", 1, glm::value_ptr(mn));
        teapotShader.setUniformMatrix4fv("v", 1, glm::value_ptr(v));
        teapotShader.setUniformMatrix4fv("mv", 1, glm::value_ptr(mv));
        teapotShader.setUniform3fv("mtl.k_d", 1, &teapot.getMeshObj().M(0).Kd[0]);
        teapotShader.setUniform3fv("mtl.k_s", 1, &teapot.getMeshObj().M(0).Ks[0]);
        teapotShader.setUniform3fv("mtl.k_a", 1, &teapot.getMeshObj().M(0).Ka[0]);
        teapotShader.setUniform1f("mtl.shine", trimesh.M(0).Ns);
        teapotShader.setUniform3fv("spotlight.li_pos", 1, glm::value_ptr(spotlight.getWorldPos()));
        teapotShader.setUniform3fv("spotlight.li_dir", 1, glm::value_ptr(spotlight.getDirection()));
        teapotShader.setUniform3fv("spotlight.color", 1, glm::value_ptr(spotlight.getColor()));
        teapotShader.setUniform1f("spotlight.radius", spotlight.getRadius());
        teapotShader.setUniform1f("spotlight.cutoff", spotlight.getCutOff());
        teapotShader.setUniform1i("tex", 0);
        teapotShader.setUniform1i("specTex", 1);

        glm::mat4 teapotMLP = matrixGen.create_mvp(teapot, li_Camera);
        glm::mat4 teapotShadowMatrix = matrixGen.create_matrix_shadow(teapotMLP, 0.00005f);
        teapotShader.setUniformMatrix4fv("TeapotShadowMatrix", 1, glm::value_ptr(teapotShadowMatrix));
   
        SceneRenderer.ArrayDraw(teapotVAO, teapotShader, teapot.getVBuffer().size());

        // Render Plane
        planeVAO.Bind();
        planeShader.Bind();
        planeShader.setUniform1i("depthMap", 2);
        planeShader.setUniformMatrix4fv("pm", 1, glm::value_ptr(pm));
        planeShader.setUniformMatrix4fv("mv", 1, glm::value_ptr(pmv));
        planeShader.setUniformMatrix4fv("v", 1, glm::value_ptr(v));
        planeShader.setUniform3fv("spotlight.li_pos", 1, &spotlight.getWorldPos());
        planeShader.setUniform3fv("spotlight.li_dir", 1, &spotlight.getDirection());
        planeShader.setUniform3fv("spotlight.color", 1, &spotlight.getColor());
        planeShader.setUniform1f("spotlight.radius", spotlight.getRadius());
        planeShader.setUniform1f("spotlight.cutoff", spotlight.getCutOff());
  
        glm::mat4 planeMLP = matrixGen.create_mvp(plane, li_Camera);
        glm::mat4 shadowMatrix = matrixGen.create_matrix_shadow(planeMLP, 0.00005f);
        planeShader.setUniformMatrix4fv("shadowMatrix", 1, glm::value_ptr(shadowMatrix));
        SceneRenderer.ArrayDraw(planeVAO, planeShader, 6);

        // Render Lighting Obj
        lightVAO.Bind();
        lightShader.Bind();
        lightShader.setUniformMatrix4fv("lm", 1, glm::value_ptr(lm));
        SceneRenderer.ArrayDraw(lightVAO, lightShader, spotlight.getVBuffer().size());

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // glViewport(0, 0, width, height);
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

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    angle += yoffset;
    angle = angle > 360.0? 0.0 : angle < 0.0? 360.0 : angle;
}

// GLuint load_env_texture(const char* filepath) {
//     if (!filepath || strlen(filepath) == 0) {
//         fprintf(stderr, "Empty environment texture path.\n");
//         exit(1);
//     }

//     std::string directory(filepath);
//     if (directory.back() != '/') {
//         directory += "/";
//     }

//     const char* last_slash = strrchr(filepath, '/');
//     std::string base_filename;
//     if (last_slash != nullptr) {
//         base_filename = std::string(last_slash + 1);
//     } else {
//         base_filename = std::string(filepath);
//     }

//     std::string negx_img = directory + base_filename + "_negx.png";
//     std::string negy_img = directory + base_filename + "_negy.png";
//     std::string negz_img = directory + base_filename + "_negz.png";
//     std::string posx_img = directory + base_filename + "_posx.png";
//     std::string posy_img = directory + base_filename + "_posy.png";
//     std::string posz_img = directory + base_filename + "_posz.png";

//     GLuint texID;
//     glGenTextures( 1, &texID );
//     glBindTexture( GL_TEXTURE_CUBE_MAP, texID );

//     std::string faces[] = {negx_img, negy_img, negz_img, posx_img, posy_img, posz_img};
//     GLenum targets[] = {
//         GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
//         GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
//         GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
//         GL_TEXTURE_CUBE_MAP_POSITIVE_X,
//         GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
//         GL_TEXTURE_CUBE_MAP_POSITIVE_Z};

//     int width, height, nrChannels;
//     for (int i = 0; i < 6; ++i) {
//         unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//         if (!data) {
//             std::cerr << "Failed to load env texture: " << faces[i] << std::endl;
//             glDeleteTextures(1, &texID);
//             exit(1);
//         }

//         GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
//         glTexImage2D(targets[i], 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         stbi_image_free(data);
//     }

//     glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     return texID;
// }