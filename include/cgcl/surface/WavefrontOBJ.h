#ifndef CGCL_SURFACE_WAVEFRONTOBJ_H
#define CGCL_SURFACE_WAVEFRONTOBJ_H

#include <cstddef>
#include <glm/glm.hpp>

#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <memory>
#include <map>

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

    std::map<std::string, int> material_indices_;
    std::vector<std::string> material_order_;
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
    void parse(std::vector<std::unique_ptr<Geometry>> &geometry, GlobalVertices &global_vertices);
    std::vector<std::string> &get_mtl_libraries() {
        return mtl_libraries_;
    }
private:
    void skipComment();
    void geom_add_vertex(GlobalVertices &global_vertices);
    void geom_add_vertex_normal(GlobalVertices &global_vertices);
    void geom_add_uv_vertex(GlobalVertices &global_vertices);
    void geom_add_polygon(Geometry *geom, GlobalVertices &global_vertices, const bool shaded_smooth);
    void geom_add_name(Geometry *geom);
    bool geom_update_smooth();

    std::vector<std::string> mtl_libraries_;
    std::string filename_;
    std::string input_;
    size_t index_ = 0;
    size_t n_line_;
};

enum class MTLTexMapType {
  Color = 0,
  Metallic,
  Specular,
  SpecularExponent,
  Roughness,
  Sheen,
  Reflection,
  Emission,
  Alpha,
  Normal,
  Count
};

struct MTLTexMap {
    bool isValid() const {
        return !image_path_.empty();
    }
    std::string image_path_;
    std::string mtl_dir_path;
};

struct MTLMaterial {
    std::string name_;
    glm::vec3 Ka_; // ambient color of material
    glm::vec3 Kd_; // diffuse color of material
    glm::vec3 Ks_; // specular color of material
    float Ns_; // specular expoent, range between 0 and 1000
    float Ni_ = 1.0f; // refraction, range from 0.001 and 10, 1.0 means no refraction
    float d = 1.0f; // transparent
    MTLTexMap tex_map_[int(MTLTexMapType::Count)];
};

class MTLParser {
public:
    MTLParser() = delete;
    MTLParser(const std::string &mtl_library, const std::string &obj_filepath);
    void parse(std::map<std::string, std::unique_ptr<MTLMaterial>> &materials);
private:
    void parseTextureMap(MTLMaterial *material);

    std::string mtl_file_path_;
    std::string mtl_dir_path_;
    std::string input_;
    size_t index_ = 0;
    size_t n_line_ = 1;
};

} // end namespace cgcl

#endif // CGCL_SURFACE_WAVEFRONTOBJ_H 