#ifndef CGCL_SURFACE_BEZIER_H
#define CGCL_SURFACE_BEZIER_H

#include <glm/glm.hpp>
#include <vector>
namespace cgcl {


class BezierSurface {
public:
    [[deprecated("The rendering of Bezier surface largely rely on fixed function pipeline now.")]]
    void render();
    void init();
    glm::vec3 getPoint(float u, float v) const;

    unsigned int n_us, n_vs;
    std::vector<glm::vec3> ctrl_pts_; // v major ctrl 
};

} // end namespace 

#endif // CGCL_SURFACE_BEZIER_H