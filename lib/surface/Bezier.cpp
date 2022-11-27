#include <cgcl/surface/Bezier.h>
#include <cgcl/math/BasicFunction.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

using namespace cgcl;

void BezierSurface::init() {
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, n_us, 
            0, 1, n_us * 3, n_vs,
            glm::value_ptr(ctrl_pts_[0]));
    glEnable(GL_MAP2_VERTEX_3);
}

void BezierSurface::render() {
    glEvalMesh2(GL_FILL, 0, 20, 0, 20);
}

glm::vec3 BezierSurface::getPoint(float u, float v) const {
    glm::vec3 p(0.0);
    for (int i = 0; i < n_us; ++i)
        for (int j = 0 ; j < n_vs; ++j) 
            p += bernstein_poly(n_us, i, u) * bernstein_poly(n_vs, j, v) * ctrl_pts_[i * n_vs + j];
    return p;
}