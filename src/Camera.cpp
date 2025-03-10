#include "Camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Camera::Camera(int windowWidth, int windowHeight) : m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
}

Camera::Camera(int windowWidth, int windowHeight, const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up) : 
    m_windowWidth(windowWidth), m_windowHeight(windowHeight),
    m_pos(pos), m_target(glm::normalize(target)), m_up(glm::normalize(up)),
    m_perspective(glm::mat4(1.0f)),
    m_angle(45.0f), m_zNear(0.01f), m_zFar(100.0f)
{
    Init();
}

Camera::Camera(int windowWidth, int windowHeight, const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up, float angle, float zNear, float zFar) :
    m_windowWidth(windowWidth), m_windowHeight(windowHeight),
    m_pos(pos), m_target(glm::normalize(target)), m_up(glm::normalize(up)),
    m_perspective(glm::mat4(1.0f)), 
    m_angle(angle), m_zNear(zNear), m_zFar(zFar)
{
    Init();
}

void Camera::Init()
{
    glm::vec3 h_target = glm::vec3(m_target.x, 0.0f, m_target.z);
    h_target = glm::normalize(h_target);

    float angle = glm::degrees(glm::asin(glm::abs(h_target.z)));

    if (h_target.z <= 0.0f) {
        if (h_target.x >= 0.0f) {
            m_angleH = angle;
        }
        else if (h_target.x < 0.0f) {
            m_angleH = 180.0f - angle;
        }
    }
    else {
        if (h_target.x >= 0.0f) {
            m_angleH = 360.0f - angle;
        }
        else if (h_target.x < 0.0f) {
            m_angleH = 180.0f + angle;
        }
    }

    m_angleV = glm::degrees(glm::asin(m_target.y));
    m_onLeftEdge = false;
    m_onRightEdge = false;
    m_onUpperEdge = false;
    m_onLowerEdge = false;
    m_mousePos.x = m_windowWidth / 2.0f;
    m_mousePos.y = m_windowHeight / 2.0f;
    Update();
    GenerateProspective();
}

void Camera::Update()
{
    glm::vec3 Vaxis(0.0f, 1.0f, 0.0f);
    glm::quat rotationQuatH = glm::angleAxis(glm::radians(m_angleH), Vaxis);
    glm::vec3 View = glm::rotate(rotationQuatH, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 Haxis = glm::normalize(glm::cross(View, Vaxis));
    glm::quat rotationQuatV = glm::angleAxis(glm::radians(m_angleV), Haxis);
    View = glm::rotate(rotationQuatV, View);
    
    View = glm::normalize(View);

    m_target = View;

    m_up = glm::normalize(glm::cross(Haxis, m_target));
    
}

bool Camera::OnKeyboard(int key)
{
    bool ret = false;

    switch (key) {
    case GLFW_KEY_W:
    {
        m_pos += (m_up * m_speed);
        ret = true;
    }
    break;

    case GLFW_KEY_S:
    {
        m_pos -= (m_up * m_speed);
        ret = true;
    }
    break;

    case GLFW_KEY_A:
    {
        glm::vec3 right = glm::normalize(glm::cross(m_target, glm::vec3(0.0f, 1.0f, 0.0f)));
        right *= m_speed;
        m_pos -= right;
        ret = true;
    }
    break;

    case GLFW_KEY_D:
    {
        glm::vec3 right = glm::normalize(glm::cross(m_target, glm::vec3(0.0f, 1.0f, 0.0f)));
        right *= m_speed;
        m_pos += right;
        ret = true;
    }
    break;
    
    }

    // std::cout << "Camera target: " << glm::to_string(m_target) << std::endl;
    // std::cout << "Camera up: " << glm::to_string(m_up) << std::endl;
    // std::cout << "Camera position: " << glm::to_string(m_pos) << std::endl;
    // glm::vec3 right = glm::normalize(glm::cross(m_target, glm::vec3(0.0f, 1.0f, 0.0f)));
    // std::cout << "Camera right: " << glm::to_string(right) << std::endl;

    return ret;
}

void Camera::OnMouse(double x, double y)
{
    double delta_X = x - m_mousePos.x;
    double delta_Y = y - m_mousePos.y;

    m_mousePos.x = x;
    m_mousePos.y = y;

    m_angleH -= delta_X / 20.0f;
    m_angleV -= delta_Y / 30.0f;

    // Clamp vertical angle to keep it between -90 and 90 degrees
    m_angleV = glm::clamp(m_angleV, -90.0f, 90.0f);

    // Edge detection and state management
    if (delta_X == 0) {
        if (x <= MARGIN) {
            m_onLeftEdge = true;
        } else if (x >= (m_windowWidth - MARGIN)) {
            m_onRightEdge = true;
        }
    } else {
        m_onLeftEdge = false;
        m_onRightEdge = false;
    }

    if (delta_Y == 0) {
        if (y <= MARGIN) {
            m_onUpperEdge = true;
        } else if (y >= (m_windowHeight - MARGIN)) {
            m_onLowerEdge = true;
        }
    } else {
        m_onUpperEdge = false;
        m_onLowerEdge = false;
    }

    Update();
}

void Camera::OnRender()
{
    bool ShouldUpdate = false;

    if (m_onLeftEdge) {
        m_angleH += 0.1f;
        ShouldUpdate = true;
    }
    if (m_onRightEdge) {
        m_angleH -= 0.1f;
        ShouldUpdate = true;
    }

    if (m_onUpperEdge) {
        if (m_angleV < 90.0f) {
            m_angleV += 0.1f;
            m_angleV = glm::clamp(m_angleV, -90.0f, 90.0f);
            ShouldUpdate = true;
        }
    }
    if (m_onLowerEdge) {
        if (m_angleV > -90.0f) {
            m_angleV -= 0.1f;
            m_angleV = glm::clamp(m_angleV, -90.0f, 90.0f);
            ShouldUpdate = true;
        }
    }

    if (ShouldUpdate) {
        Update();
    }
}



void Camera::OnRelease(double x, double y)
{
    m_mousePos.x = x;
    m_mousePos.y = y;

    m_onLeftEdge = false;
    m_onRightEdge = false;
    m_onUpperEdge = false;
    m_onLowerEdge = false;
}

void Camera::OnDrag(double x, double y)
{
    double delta_Y = y - m_mousePos.y;

    m_mousePos.x = x;
    m_mousePos.y = y;

    m_pos += (m_target * (float)delta_Y * (m_speed * 0.01f));
    Update();
}

const glm::mat4 &Camera::GetProspective() const
{
    return m_perspective;
}

const glm::vec3 &Camera::GetPos() const
{
    return m_pos;
}

const glm::vec3 &Camera::GetTarget() const
{
    return m_target;
}

const glm::vec3 &Camera::GetUp() const
{
    return m_up;
}

void Camera::GenerateProspective()
{
    float aspectRatio = static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight);
    glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(m_angle), aspectRatio, m_zNear, m_zFar);
    m_perspective = perspectiveMatrix;
}
