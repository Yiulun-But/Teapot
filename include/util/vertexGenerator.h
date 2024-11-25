#include "cy/cyTriMesh.h"

class vertexGenerator
{
    public:
        vertexGenerator() {};
        inline void generateBuffers(cy::TriMesh& trimesh, std::vector<cy::Vec3f>& vBuffer, std::vector<cy::Vec3f>& tBuffer, std::vector<cy::Vec3f>& nBuffer) {
            vBuffer.clear();
            tBuffer.clear();
            nBuffer.clear();
            for (int i=0; i<trimesh.NF(); ++i) {
                vBuffer.push_back(trimesh.V(trimesh.F(i).v[0]));
                vBuffer.push_back(trimesh.V(trimesh.F(i).v[1]));
                vBuffer.push_back(trimesh.V(trimesh.F(i).v[2]));
                if (trimesh.HasTextureVertices()) {
                    tBuffer.push_back(trimesh.VT(trimesh.FT(i).v[0]));
                    tBuffer.push_back(trimesh.VT(trimesh.FT(i).v[1]));
                    tBuffer.push_back(trimesh.VT(trimesh.FT(i).v[2]));
                }
                if (trimesh.HasNormals()) {
                    nBuffer.push_back(trimesh.VN(trimesh.FN(i).v[0]));
                    nBuffer.push_back(trimesh.VN(trimesh.FN(i).v[1]));
                    nBuffer.push_back(trimesh.VN(trimesh.FN(i).v[2]));
                }
            }
        };

};