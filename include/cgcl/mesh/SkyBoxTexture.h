#pragma once

#include "cgcl/mesh/Texture.h"

#include <memory>
#include <vector>
#include <string>

namespace cgcl {

class TriMesh;

/// \brief Skybox read cubic map and hold a texture id;
class SkyBoxTexture : public Texture {
public:
    /// \attention: This method does not override Texture::LoadTexture,
    /// as a bounding box need 6 picture to construct a cubic_map.
    virtual void LoadTexture(const std::vector<std::string> &box_faces);
    virtual void BindTexture() const override;


};

} // end namespace cgcl