#include "cgcl/surface/WavefrontOBJ.h"
#include "cgcl/utils/logging.h"

#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <cstring>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace cgcl;


static inline bool is_whitespace(char c) {
    return c <= ' '; // treate ASCII control chars as white space.
}

static std::size_t skipWhiteSpace(const std::string &input, size_t &index) {
    size_t new_lines = 0;
    for (; index < input.size() && is_whitespace(input[index]); ++index) {
        if ((index + 1 < input.size() && input[index] == '\r' && input[index + 1] == '\n') ||
          input[index] == '\r' || input[index] == '\n')
        new_lines++;
    }
    return new_lines;
}

void OBJParser::skipComment() {
    for (; index_ < input_.size() && input_[index_] != '\n'; index_++); 
}

static size_t tryParseFloat(const std::string &input, size_t index, float &dst) {
    size_t length = 0;
    if (index < input.size()) {
        char *end = nullptr;
        dst = strtof(input.data() + index, &end);
        length = end - input.data() - index;
    }
    CHECK_NE(length, 0) << "parse float error at " << index;
    return index + length;
}


static size_t tryParseInt(const std::string &input, size_t index, int &dst) {
    size_t length = 0;
    if (index < input.size()) {
        char *end = nullptr;
        dst = strtol(input.data() + index, &end, 0);
        length =  end - input.data() - index;
    }
    CHECK_NE(length, 0) << "parse int error at " << index;
    return index + length;
}


static size_t tryParseString(const std::string &input, size_t index, std::string_view &name) {
    size_t name_end = input.find('\n', index);
    CHECK_NE(name_end, std::string::npos) << "Expect name";
    name = std::string_view(input).substr(index, name_end-index);
    return name_end;
}

static bool startWith(const std::string &input, size_t index, const std::string_view s) {
    return (input.size() - index >= s.length()) && 
        (memcmp(s.data(), input.data() + index, s.length()) == 0);
}

static bool expectKeyword(const std::string &input, size_t &index, const std::string_view keyword) {
    size_t keyword_len = keyword.size();
    if (input.size() - index < keyword_len + 1) {
        return false;
    }
    if (memcmp(input.data() + index, keyword.data(), keyword.size()) != 0) {
        return false;
    }
    if (input[index + keyword_len] > ' ') {
        return false;
    }
    index += keyword_len + 1;
    return true;
}

void OBJParser::parse(Geometry &geometry, GlobalVertices &global_vertices) {
    
    std::ifstream input_stream(filename_);
    if (!input_stream.good()) {
        std::cerr << "Can not open " << filename_ << std::endl;
        return;
    } 
    
    std::getline(input_stream, input_, '\0');

    bool state_smooth = false;
    n_line_ = 1;
    for (;index_ < input_.size(); ) {
        n_line_ += skipWhiteSpace(input_, index_);
        if (input_[index_] == '#')
            skipComment();
        else if (input_[index_] == 'v') {
            if (expectKeyword(input_, index_, "v")) {
                geom_add_vertex(global_vertices);
            } else if (expectKeyword(input_, index_, "vt")) {
                geom_add_uv_vertex(global_vertices);
            } else if (expectKeyword(input_, index_, "vn")) {
                geom_add_vertex_normal(global_vertices);
            }
        }
        else if (input_[index_] == 'f') {
            if (expectKeyword(input_, index_, "f")) {
                geom_add_polygon(geometry, global_vertices, state_smooth);
            }
        }
        else if (input_[index_] == 'o') {
            if (expectKeyword(input_, index_, "o")) {
                geom_add_name(geometry);
            }
        }
        else if (input_[index_] == 's') {
            if (expectKeyword(input_, index_, "s")) {
                state_smooth = geom_update_smooth();
            }
        }
        /* Material */
        else if (expectKeyword(input_, index_, "usemtl")) {
            
        }
    }

    LOG(INFO) << "Read from: " << filename_;
    LOG(INFO) << "Total Vertex: " << global_vertices.vertices.size();
    LOG(INFO) << "Total Faces: " << geometry.face_elements_.size();
}

void OBJParser::geom_add_name(Geometry &geom) {
    size_t name_end = input_.find('\n', index_);
    if (name_end != std::string::npos) {
        geom.geometry_name_ = input_.substr(index_, name_end - index_);
        index_ = name_end;
    }
}

bool OBJParser::geom_update_smooth() {
    size_t end_line = input_.find('\n', index_);
    std::string_view line(input_.c_str() + index_, end_line - index_);
    if (line == "0" || line == "off" || line == "null") {
        index_ = end_line;
        return false;
    }

    int smooth = 0;
    index_ = tryParseInt(input_, index_, smooth);
    return smooth != 0;
}

#define parse_floats(input, index, p, count)                                            \
    for (int i = 0; i < count; ++i) {                                                   \
        index = tryParseFloat(input, index, p[i]);                                     \
    }                                                                                   \

void OBJParser::geom_add_vertex(GlobalVertices &global_vertices) {
    glm::vec3 vert;
    parse_floats(input_, index_, glm::value_ptr(vert), 3);
    global_vertices.vertices.push_back(vert);
    // If there is newline after xyz, parse finished.
    // Otherwise, parse rgb data.
    if (auto new_lines = skipWhiteSpace(input_, index_)) {
        n_line_ += new_lines;
        return;
    }

}

void OBJParser::geom_add_vertex_normal(GlobalVertices &global_vertices) {
    glm::vec3 normal;
    parse_floats(input_, index_, glm::value_ptr(normal), 3);
    glm::normalize(normal);
    global_vertices.vertex_normals.push_back(normal);
}

void OBJParser::geom_add_uv_vertex(GlobalVertices &global_vertices) {
    glm::vec2 uv;
    parse_floats(input_, index_, glm::value_ptr(uv), 2);
    global_vertices.uv_vertices.push_back(uv);
}

void OBJParser::geom_add_polygon(Geometry &geom, GlobalVertices &global_vertices,
                                 const bool shaded_smooth) 
{
    PolyElem curr_face;
    curr_face.shaded_smooth_ = shaded_smooth;

    const int orig_corners_size = geom.face_corners_.size();
    curr_face.start_index_ = orig_corners_size;

    bool face_valid = true;
    /* Parse until new line*/
    for (;;) {
        if (auto new_lines = skipWhiteSpace(input_, index_)) {
            n_line_ += new_lines;
            break;
        }

        PolyCorner corner;
        bool got_uv = false, got_normal = false;
        index_ = tryParseInt(input_, index_, corner.vert_index);
        if (input_[index_] == '/') {
            ++index_;
            /* UV index */
            if (input_[index_] != '/') {
                index_ = tryParseInt(input_, index_, corner.uv_vert_index);
                got_uv = true;
            }
            /* normal index */
            if (input_[index_] == '/') {
                ++index_;
                index_ = tryParseInt(input_, index_, corner.vertex_normal_index);
                got_normal = true;
            }
        }
        /* Keep vertex index zero-based */
        corner.vert_index += -1;
        CHECK_GE(corner.vert_index, 0);
        CHECK_LT(corner.vert_index, global_vertices.vertices.size());
        geom.track_vertex_index(corner.vert_index);

        if (got_uv) {
            corner.uv_vert_index += -1;
            CHECK_GE(corner.uv_vert_index, 0);
            CHECK_LT(corner.uv_vert_index, global_vertices.uv_vertices.size());
        }

        /* Ignore corner normal index, if the geometry does not have any normals.
         * Some obj files out there do have face definitions that refer to normal indices,
         * without any normals being present (T98782). */
        if (got_normal && !global_vertices.vertex_normals.empty()) {
            corner.vertex_normal_index += -1;
            CHECK_GE(corner.vertex_normal_index, 0);
            CHECK_LT(corner.vertex_normal_index, global_vertices.vertex_normals.size());
        }
        geom.face_corners_.push_back(corner);
        curr_face.corner_count_++;

    }

    geom.face_elements_.push_back(curr_face);
    
}


void MTLParser::parse(std::map<std::string, std::unique_ptr<MTLMaterial>> &materials) {

}