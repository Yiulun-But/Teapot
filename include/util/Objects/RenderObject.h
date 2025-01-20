#ifndef RENDEROBJ_H
#define RENDEROBJ_H

#include "Object.h"
#include <cy/cyTriMesh.h>
#include <glm/gtc/matrix_transform.hpp>

class RenderObject : public Object{
public:
    RenderObject(glm::vec3 worldPos) : Object(worldPos){  };
    RenderObject(glm::vec3 worldPos, glm::mat4 scale, glm::mat4 rotate, glm::mat4 trans, cy::TriMesh& mesh) : Object(worldPos, scale, rotate, trans, mesh) {
        m_hasMesh = true;
    };



protected:
    void calculateModelMatrix() override {
        glm::mat4 originMatrix = glm::mat4(1.0f);
        if (m_hasMesh) {
                m_objMesh.ComputeBoundingBox();
                cy::Vec3f boundCenter = (m_objMesh.GetBoundMin() + m_objMesh.GetBoundMax()) * 0.5f;
                cy::Vec3f boxVolumn = (m_objMesh.GetBoundMax() - m_objMesh.GetBoundMin()).Abs();
                originMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-boundCenter.x, -boundCenter.y, -boundCenter.z));
        }
        m_modelMatrix =  m_translation * m_rotation * m_scale * originMatrix;
    };
};

#endif