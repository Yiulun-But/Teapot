#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cy/cyTriMesh.h>
#include "Camera.h"
#include "spotLight.h"
#include <glm/gtx/string_cast.hpp>
#include "util/Objects/RenderObject.h"

class MatrixGenerator {
public:
    MatrixGenerator(int windowWidth, int windowHeight)
        : m_WindowWidth(windowWidth), m_WindowHeight(windowHeight) {
        }

    glm::mat4 create_scale(glm::vec3 scales);
    glm::mat4 create_rotation(float angle, glm::vec3 axis);
    glm::mat4 create_translation(glm::vec3 trans);
    glm::mat4 create_perspective(float angle, float zNear, float zFar);

    glm::mat4 create_matrix_shadow(glm::mat4 mlp, float bias);

    glm::mat4 create_mvp(Object& object, Camera& viewCamera);
    glm::mat4 create_mv(Object& object, Camera& viewCamera);
    glm::mat4 create_mn(glm::mat4 mv);
    glm::mat4 create_vp(Camera& viewCamera);
    glm::mat4 create_v(Camera& viewCamera);

    glm::mat4 light_rotation_matrix(glm::vec3 light_pos, glm::vec3 teapot_pos);

private:
    int m_WindowWidth;
    int m_WindowHeight;

};
