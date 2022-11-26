#ifndef CGCL_SURFACE_WAVEFRONTOBJ_H
#define CGCL_SURFACE_WAVEFRONTOBJ_H

#include <cstddef>
#include <glm/glm.hpp>

#include <vector>
#include <set>
#include <string>
#include <string_view>

namespace cgcl {

/// \brief All vertex position, normals, UVs, colors in the OBJ file.
struct GlobalVertices {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uv_vertices;
    std::vector<glm::vec3> vertex_normals;

    /// \brief Vertex colors is optional in file.colors
    /// Store them in chunks as they are spelled out in 
    /// the file.
    struct VertexColorsBlock {
        std::vector<glm::vec3> colors;
        int start_vertex_index;
    };
    std::vector<VertexColorsBlock> vertex_colors;
};

struct PolyCorner {
    /* range from zero to total verteces in the OBJ file. */
    int vert_index;
    /* -1 means absence */
    int uv_vert_index = -1;
    int vertex_normal_index = -1;
};

struct PolyElem {
    int start_index_ = 0;
    int corner_count_ = 0; 
    bool shaded_smooth_ = false;
};

struct Geometry {
    std::string geometry_name_;
    std::set<int> vertices_;
    int vertex_index_min_ = INT_MAX;
    int vertex_index_max_ = -1;
    std::vector<PolyCorner> face_corners_;
    std::vector<PolyElem> face_elements_;
    void track_vertex_index(int index) {
        vertices_.insert(index);
        if (vertex_index_min_ > index) vertex_index_min_ = index;
        if (vertex_index_max_ < index) vertex_index_max_ = index;
    }
};


class OBJParser {
public:
    OBJParser() = delete;
    OBJParser(const std::string filename) : filename_(filename) {}
    void parse(Geometry &geometry, GlobalVertices &global_vertices);
private:
    size_t skipWhiteSpace();
    void skipComment();
    bool expectKeyword(const std::string_view keyword);
    void tryParseInt(int &dst);
    void tryParseFloat(float &dst);
    bool startWith(const std::string_view s);
    void geom_add_vertex(GlobalVertices &global_vertices);
    void geom_add_vertex_normal(GlobalVertices &global_vertices);
    void geom_add_uv_vertex(GlobalVertices &global_vertices);
    void geom_add_polygon(Geometry &geom, GlobalVertices &global_vertices, const bool shaded_smooth);
    void geom_add_name(Geometry &geom);
    bool geom_update_smooth();

    std::string filename_;
    std::string input_;
    size_t index_;
    size_t n_line_;
};

} // end namespace cgcl

#endif // CGCL_SURFACE_WAVEFRONTOBJ_H 