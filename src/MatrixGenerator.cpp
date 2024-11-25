#include "MatrixGenerator.h"

glm::mat4 MatrixGenerator::create_matrix(cy::TriMesh& trimesh)
{
    trimesh.ComputeBoundingBox();
    cy::Vec3f boundCenter = (trimesh.GetBoundMin() + trimesh.GetBoundMax()) * 0.5f;

    static float radian = 0.0f;
    static float delta = 0.000f;
    radian += delta;

    glm::mat4 scaleMatrix;
    if (m_Camera->GetPerspection()) {
        scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(-0.25f, -0.25f, 0.25f));
    } else {
        float cameraDistance = glm::length(m_Camera->GetPos() - glm::vec3(0.25f * boundCenter.x, 1.25f * boundCenter.y, 0.25f * boundCenter.z + 7.0f));
        float scaleFactor = 1.0f / (cameraDistance + 0.01f);
        scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(-scaleFactor, -scaleFactor, scaleFactor));
    }

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radian, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f * boundCenter.x, 0.25f * boundCenter.y, -0.25f * boundCenter.z + 7.0f));
    glm::mat4 viewMatrix = create_view();
    glm::mat4 projectionMatrix;

    if (m_Camera->GetPerspection()) {
        float FOV = 90.0f;
        projectionMatrix = create_perspective(FOV, 0.1f, 50.0f);
    } else {
        float orthoSize = 2.0f;
        float aspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
        projectionMatrix = glm::ortho(-orthoSize * aspectRatio,
                                      orthoSize * aspectRatio,
                                      -orthoSize,
                                      orthoSize,
                                      0.1f,
                                      50.0f);
    }

    glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    glm::mat4 result = projectionMatrix * viewMatrix * modelMatrix;
    mvp = result;
    mv = viewMatrix * modelMatrix;
    glm::mat3 mv3(mv);
    mn = glm::transpose(glm::inverse(mv3));
    created = true;

    return result;
}

glm::mat4 MatrixGenerator::create_perspective(float angle, float zNear, float zFar)
{
    float aspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
    glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(angle), aspectRatio, zNear, zFar);
    return perspectiveMatrix;
}

glm::mat4 MatrixGenerator::create_view()
{
    glm::vec3 forwardVec = m_Camera->GetTarget();
    glm::vec3 upVec = m_Camera->GetUp();
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
        -m_Camera->GetPos().x,
        -m_Camera->GetPos().y,
        -m_Camera->GetPos().z,
        1.0f
    );

    return viewMatrix1 * viewMatrix2;
}
