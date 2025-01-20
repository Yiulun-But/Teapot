#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "LightObject.h"
#include <cy/cyTriMesh.h>
#include <glm/gtc/matrix_transform.hpp>

class SpotLight : public LightObject {
public:
    SpotLight(glm::vec3 worldPos) : LightObject(worldPos) {  };
    SpotLight(glm::vec3 worldPos, glm::mat4 scale, glm::mat4 rotate, glm::mat4 trans, float radius, glm::vec3 color, glm::vec3 dir, float cutoff, cy::TriMesh& mesh) : 
        LightObject(worldPos, scale, rotate, trans, radius, color, mesh),
        m_dir(dir),
        m_cutoff(cutoff) {
        m_hasMesh = true;
    };

    inline void setDirection(glm::vec3 dir) { m_dir = dir; };
    inline void setCutOff(float cutoff) { m_cutoff = cutoff; };

    inline glm::vec3& getDirection() { return m_dir; };
    inline float& getCutOff() { return m_cutoff; };

protected:
    glm::vec3 m_dir;
    float m_cutoff;
};

#endif