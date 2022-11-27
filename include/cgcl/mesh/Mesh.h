#pragma once

#include "cgcl/surface/Bezier.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

namespace cgcl {

struct Vertex {
    glm::vec3 position_;
    glm::vec3 normal_;
    glm::vec2 texture_coords_;
};


/// \brief Triangular mesh.
class TriMesh {
public:
    TriMesh() = delete;
    static std::shared_ptr<TriMesh> from_obj(const std::string &filename);
    static std::shared_ptr<TriMesh> from_bezier(const BezierSurface &bezier);
    /* A mesh may contain multiple sub-mesh and its own
     * vertex, index, uv and material, texture.
     */
    std::vector<Vertex> global_vertices_;
    std::vector<unsigned int> global_indices_;

    // support one sub-mesh only.
    // std::vector<unsigned int> offsets_;
    // std::vector<unsigned int> counts_;
protected:
    TriMesh(const std::vector<Vertex> &vertex, const std::vector<unsigned int> &indices)
        : global_vertices_(vertex), global_indices_(indices) {}
    TriMesh(std::vector<Vertex> &&vertex, std::vector<unsigned int> &&indices)
        : global_vertices_(vertex), global_indices_(indices) {}
private:
    void initGL();
    void finishGL();
    bool need_rendering_ = false;
    unsigned int VAO, VBO, EBO;

};

} // end namespace cgcl