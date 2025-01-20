#ifndef OBJ_H
#define OBJ_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <cy/cyTriMesh.h>

class Object {
public:
    Object() {};
    Object(glm::vec3 worldPos) : m_worldPos(worldPos) {};
    Object(glm::vec3 worldPos, glm::mat4 scale, glm::mat4 rotate, glm::mat4 trans) : 
        m_worldPos(worldPos), 
        m_scale(scale), 
        m_rotation(rotate), 
        m_translation(trans)
        {
    };
    Object(glm::vec3 worldPos, glm::mat4 scale, glm::mat4 rotate, glm::mat4 trans, cy::TriMesh mesh) : 
        m_worldPos(worldPos), 
        m_scale(scale), 
        m_rotation(rotate), 
        m_translation(trans),
        m_objMesh(mesh)
        {
        m_hasMesh = true;
    };

    inline void setMesh(cy::TriMesh& mesh) { m_objMesh = mesh; };

    inline void setWorldPos(glm::vec3 pos) { m_worldPos = pos; };
    inline void setWorldScale(glm::mat4 scale) { m_scale = scale; };
    inline void setWorldRotate(glm::mat4 rotate) { m_rotation = rotate; };
    inline void setWorldTrans(glm::mat4 trans) { m_translation = trans; };
    
    inline glm::vec3& getWorldPos() { return m_worldPos; };
    cy::TriMesh& getMeshObj() { return m_objMesh; };
    inline glm::mat4 getModelMatrix() { calculateModelMatrix(); return m_modelMatrix; };
    inline std::vector<cy::Vec3f>& getVBuffer() { return m_vBuffer; };
    inline std::vector<cy::Vec2f>& getTBuffer() { return m_tBuffer; };
    inline std::vector<cy::Vec3f>& getNBuffer() { return m_nBuffer; };


    inline void loadBuffers() {
        m_vBuffer.clear();
        m_tBuffer.clear();
        m_nBuffer.clear();
        for (int i=0; i<m_objMesh.NF(); ++i) {
            m_vBuffer.push_back(m_objMesh.V(m_objMesh.F(i).v[0]));
            m_vBuffer.push_back(m_objMesh.V(m_objMesh.F(i).v[1]));
            m_vBuffer.push_back(m_objMesh.V(m_objMesh.F(i).v[2]));
            if (m_objMesh.HasTextureVertices()) {
                cy::Vec2f vec1(m_objMesh.VT(m_objMesh.FT(i).v[0])[0], m_objMesh.VT(m_objMesh.FT(i).v[0])[1]);
                m_tBuffer.push_back(vec1);
                cy::Vec2f vec2(m_objMesh.VT(m_objMesh.FT(i).v[1])[0], m_objMesh.VT(m_objMesh.FT(i).v[1])[1]);
                m_tBuffer.push_back(vec2);
                cy::Vec2f vec3(m_objMesh.VT(m_objMesh.FT(i).v[2])[0], m_objMesh.VT(m_objMesh.FT(i).v[2])[1]);
                m_tBuffer.push_back(vec3);
            }
            if (m_objMesh.HasNormals()) {
                m_nBuffer.push_back(m_objMesh.VN(m_objMesh.FN(i).v[0]));
                m_nBuffer.push_back(m_objMesh.VN(m_objMesh.FN(i).v[1]));
                m_nBuffer.push_back(m_objMesh.VN(m_objMesh.FN(i).v[2]));
            }
        }
    }

protected:

    glm::vec3 m_worldPos = glm::vec3(0.0f);
    glm::mat4 m_scale = glm::mat4(1.0f);
    glm::mat4 m_rotation = glm::mat4(1.0f);
    glm::mat4 m_translation = glm::mat4(1.0f);
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    bool m_hasMesh = false;
    cy::TriMesh m_objMesh;
    std::vector<cy::Vec3f> m_vBuffer;
    std::vector<cy::Vec2f> m_tBuffer;
    std::vector<cy::Vec3f> m_nBuffer;

    virtual void calculateModelMatrix() {
        m_modelMatrix = m_translation * m_rotation * m_scale;
    };
};

#endif