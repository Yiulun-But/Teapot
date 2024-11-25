#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


#define MARGIN 20

class Camera
{
public:
    Camera(int windowWidth, int windowHeight);
    Camera(int windowWidth, int windowHeight, const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);
    bool OnKeyboard(int key);
    void OnMouse(double x, double y);
    void OnRelease(double x, double y);
    void OnDrag(double x, double y);
    void OnRender();
    const glm::vec3& GetPos() const;
    const glm::vec3& GetTarget() const;
    const glm::vec3& GetUp() const;
    const bool& GetPerspection() const;

private:
    int m_windowWidth;
    int m_windowHeight;

    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_speed = 1.0f;

    float m_angleH;
    float m_angleV;

    bool m_onUpperEdge;
    bool m_onLowerEdge;
    bool m_onLeftEdge;
    bool m_onRightEdge;

    glm::vec2 m_mousePos;
    bool m_perspective;

    void Init();
    void Update();
};