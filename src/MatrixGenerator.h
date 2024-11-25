#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cy/cyTriMesh.h>
#include "Camera.h"

class MatrixGenerator {
public:
    MatrixGenerator(int windowWidth, int windowHeight, Camera* camera)
        : m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_Camera(camera) {}

    inline glm::mat4 MVP() { return created? mvp : glm::mat4(1.0f);};
    inline glm::mat4 MV() { return created? mv : glm::mat4(1.0f);};
    inline glm::mat4 MN() { return created? mn : glm::mat3(1.0f);};
    glm::mat4 create_matrix(cy::TriMesh& trimesh);

private:
    glm::mat4 create_perspective(float angle, float zNear, float zFar);
    glm::mat4 create_view();

    int m_WindowWidth;
    int m_WindowHeight;
    Camera* m_Camera;
    glm::mat4 mvp;
    glm::mat4 mv;
    glm::mat3 mn;
    bool created = false;
};
