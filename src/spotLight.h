#include "cy/cyTriMesh.h"
#include <glm/glm.hpp>



class spotLight {
public:
    spotLight() {};
    spotLight(cy::TriMesh& lightMesh) : lightobj(lightMesh){};

    inline void setPos(glm::vec3 pos) { m_pos = pos; };
    inline void setDir(glm::vec3 dir) { m_dir = dir; };
    inline void setCutoff(float cutoff) { m_cutoff = cutoff; };
    inline void setRadius(float radius) { m_radius = radius; };
    inline void setColor(glm::vec3 color) { m_color = color; };
    inline void setUp(glm::vec3 up) { m_up = up; }; 
    inline glm::vec3 getPos() { return m_pos; };
    inline glm::vec3 getDir() { return m_dir; };
    inline float getCutoff() { return m_cutoff; };
    inline cy::TriMesh& getObj() { return lightobj; };
    inline float getRadius() { return m_radius; };
    inline glm::vec3 getColor() { return m_color; };
    inline glm::vec3 getUp() { return m_up; };
private:
    glm::vec3 m_pos;
    glm::vec3 m_dir;
    glm::vec3 m_up;
    float m_cutoff;
    cy::TriMesh lightobj;
    float m_radius;
    glm::vec3 m_color;
};
