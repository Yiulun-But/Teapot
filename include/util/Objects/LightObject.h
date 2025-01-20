#ifndef LIGHTOBJ_H
#define LIGHTOBJ_H

#include "Object.h"
#include <cy/cyTriMesh.h>
#include <glm/gtc/matrix_transform.hpp>

class LightObject : public Object {
public:
    LightObject(glm::vec3 worldPos) : Object(worldPos) {  };
    LightObject(glm::vec3 worldPos, glm::mat4 scale, glm::mat4 rotate, glm::mat4 trans, float radius, glm::vec3 color, cy::TriMesh& mesh) : 
        Object(worldPos, scale, rotate, trans, mesh),
        m_radius(radius),
        m_color(color)
         {
        m_hasMesh = true;
    };

    inline void setRadius(float radius) { m_radius = radius; };
    inline void setColor(glm::vec3 color) { m_color = color; };

    inline float& getRadius() { return m_radius; };
    inline glm::vec3& getColor() { return m_color; };

protected:
    float m_radius;
    glm::vec3 m_color;

};

#endif