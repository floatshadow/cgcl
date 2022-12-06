#pragma once 

#include "cgcl/platform/OpenGL/GLShader.h"
#include <glm/glm.hpp>

namespace cgcl {


/// \brief Bare material with no lighting texture,
/// only for testing bling-phong lighting model.
class PhongMaterial {
public:
    PhongMaterial(const glm::vec3 &color, float Ka_coef = 1.0f, float Kd_coef = 1.0f, float Ks_coef = 0.5f,
                  float decay = 16.0f)
        : Ka_(color * Ka_coef), Kd_(color * Kd_coef), Ks_(color * Ks_coef), decay_(decay) {}
    PhongMaterial(const glm::vec3 &Ka, const glm::vec3 &Kd, const glm::vec3 &Ks,
                 float decay = 16.0f)
        : Ka_(Ka), Kd_(Kd), Ks_(Ks), decay_(decay) {} 
    void updateBareMaterial(GLShader &shader);
    void updateLightingMapMaterial(GLShader &shader, unsigned int texture_group_id);

    glm::vec3 Ka_;
    glm::vec3 Kd_;
    glm::vec3 Ks_;
    float decay_;
};


} // end namespace cgcl