#include "cgcl/mesh/PhongMaterial.h"

using namespace cgcl;

void PhongMaterial::updateBareMaterial(GLShader &shader) {
    shader.updateUniformFloat3("material.Ka", Ka_);
    shader.updateUniformFloat3("material.Kd", Kd_);
    shader.updateUniformFloat3("material.Ks", Ks_);
    shader.updateUniformFloat("material.highlight_decay", decay_);

}


void PhongMaterial::updateLightingMapMaterial(GLShader &shader, unsigned int texture_group_id) {
    shader.updateUniformInt("material.diffuse", texture_group_id);
    shader.updateUniformFloat3("material.Ks", Ks_);
    shader.updateUniformFloat("material.highlight_decay", decay_);
}