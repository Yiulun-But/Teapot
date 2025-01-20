#include "MatrixGenerator.h"



glm::mat4 MatrixGenerator::create_perspective(float angle, float zNear, float zFar)
{
    float aspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
    glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(angle), aspectRatio, zNear, zFar);
    return perspectiveMatrix;
}


glm::mat4 MatrixGenerator::create_matrix_shadow(glm::mat4 mlp, float bias)
{
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f - bias));
    return translation * scale * mlp;
}


glm::mat4 MatrixGenerator::light_rotation_matrix(glm::vec3 light_pos, glm::vec3 teapot_pos)
{
    glm::vec3 direction = glm::normalize(teapot_pos - light_pos);

    glm::vec3 default_up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 up = glm::abs(glm::dot(direction, default_up)) > 0.999f
                   ? glm::vec3(0.0f, 0.0f, 1.0f)
                   : default_up;

    glm::vec3 right = glm::normalize(glm::cross(up, direction));

    glm::vec3 orthogonal_up = glm::cross(direction, right);

    glm::mat4 rotation_matrix(1.0f); 
    rotation_matrix[0] = glm::vec4(right, 0.0f); 
    rotation_matrix[1] = glm::vec4(orthogonal_up, 0.0f);
    rotation_matrix[2] = glm::vec4(-direction, 0.0f);

    return rotation_matrix;
}
glm::mat4 MatrixGenerator::create_scale(glm::vec3 scales)
{
    return glm::scale(glm::mat4(1.0f), scales);
}

glm::mat4 MatrixGenerator::create_rotation(float angle, glm::vec3 axis)
{
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

glm::mat4 MatrixGenerator::create_translation(glm::vec3 trans)
{
    return glm::translate(glm::mat4(1.0f), trans);
}

glm::mat4 MatrixGenerator::create_mvp(Object& object, Camera &viewCamera)
{
    glm::mat4 viewMatrix = glm::lookAt(viewCamera.GetPos(), viewCamera.GetTarget() + viewCamera.GetPos(), viewCamera.GetUp());
    return viewCamera.GetProspective() * viewMatrix * object.getModelMatrix();
}

glm::mat4 MatrixGenerator::create_mv(Object &object, Camera &viewCamera)
{
    glm::mat4 viewMatrix = glm::lookAt(viewCamera.GetPos(), viewCamera.GetTarget() + viewCamera.GetPos(), viewCamera.GetUp());
    return viewMatrix * object.getModelMatrix();
}

glm::mat4 MatrixGenerator::create_v(Camera &viewCamera)
{
    return glm::lookAt(viewCamera.GetPos(), viewCamera.GetTarget() + viewCamera.GetPos(), viewCamera.GetUp());
}

glm::mat4 MatrixGenerator::create_mn(glm::mat4 mv)
{
    glm::mat3 mv3(mv);
    return glm::transpose(glm::inverse(mv3));
}

glm::mat4 MatrixGenerator::create_vp(Camera &viewCamera)
{
    glm::mat4 viewMatrix = glm::lookAt(viewCamera.GetPos(), viewCamera.GetTarget() + viewCamera.GetPos(), viewCamera.GetUp());
    return viewCamera.GetProspective() * viewMatrix;
}
