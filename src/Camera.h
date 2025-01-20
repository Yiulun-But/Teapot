#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#define MARGIN 50

#ifndef CAMERA_H
#define CAMERA_H
class Camera
{
public:
    Camera() {};
    Camera(int windowWidth, int windowHeight);
    Camera(int windowWidth, int windowHeight, const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);
    Camera(int windowWidth, int windowHeight, const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up, float angle, float zNear, float zFar);
    bool OnKeyboard(int key);
    void OnMouse(double x, double y);
    void OnRelease(double x, double y);
    void OnDrag(double x, double y);
    void OnRender();
    const glm::vec3& GetPos() const;
    const glm::vec3& GetTarget() const;
    const glm::vec3& GetUp() const;

    inline void SetPos(glm::vec3 pos) { m_pos =  pos; };
    inline void SetTarget(glm::vec3 target) { m_target = target; };
    inline void SetUp(glm::vec3 up) { m_up = up; };

    void GenerateProspective();
    const glm::mat4& GetProspective() const;

    inline void setAngle(float angle) { m_angle = angle; };
    inline void setZNear(float zNear) { m_zNear = zNear; };
    inline void setZFar(float zFar) { m_zFar = zFar; };

    inline float getAngle() { return m_angle; };
    inline float getZNear() { return m_zNear; };
    inline float getZFar() { return m_zFar; };

    glm::vec3 m_pos;
    glm::vec3 m_target;
    glm::vec3 m_up;
private:
    int m_windowWidth;
    int m_windowHeight;

    float m_speed = 1.0f;

    float m_angleH;
    float m_angleV;

    bool m_onUpperEdge;
    bool m_onLowerEdge;
    bool m_onLeftEdge;
    bool m_onRightEdge;

    glm::vec2 m_mousePos;

    glm::mat4 m_perspective;
    float m_angle;
    float m_zNear;
    float m_zFar;

    void Init();
    void Update();
};

#endif