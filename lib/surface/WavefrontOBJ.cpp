#include "cgcl/surface/WavefrontOBJ.h"
#include "cgcl/utils/logging.h"

#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
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

std::size_t OBJParser::skipWhiteSpace() {
    size_t new_lines = 0;
    for (; index_ < input_.size() && is_whitespace(input_[index_]); ++index_) {
        if ((index_ + 1 < input_.size() && input_[index_] == '\r' && input_[index_ + 1] == '\n') ||
          input_[index_] == '\r' || input_[index_] == '\n')
        new_lines++;
    }
    return new_lines;
}

void OBJParser::skipComment() {
    for (; index_ < input_.size() && input_[index_] != '\n'; index_++); 
}

void OBJParser::tryParseFloat(float &dst) {
    size_t length = 0;
    if (index_ < input_.size()) {
        char *end = nullptr;
        strtof(input_.data() + index_, &end);
        length = end - input_.data() - index_;
    }
    CHECK_NE(length, 0) << "parse float error at " << filename_ << ":" << n_line_; 
    index_ += length;
}


void OBJParser::tryParseInt(int &dst) {
    size_t length = 0;
    if (index_ < input_.size()) {
        char *end = nullptr;
        dst = strtol(input_.data() + index_, &end, 0);
        length =  end - input_.data() - index_;
    }
    CHECK_NE(length, 0) << "parse int error at " << filename_ << ":" << n_line_; 
    index_ += length;
}


bool OBJParser::startWith(const std::string_view s) {
    return (input_.size() - index_ >= s.length()) && 
        (memcmp(s.data(), input_.data() + index_, s.length()) == 0);
}

bool OBJParser::expectKeyword(const std::string_view keyword) {
    size_t keyword_len = keyword.size();
    if (input_.size() - index_ < keyword_len + 1) {
        return false;
    }
    if (memcmp(input_.data() + index_, keyword.data(), keyword.size()) != 0) {
        return false;
    }
    if (input_[keyword_len] > ' ') {
        return false;
    }
    index_ += keyword_len + 1;
    return true;
}

void OBJParser::parse(Geometry &geometry, GlobalVertices &global_vertices) {
    
    std::ifstream input_stream(filename_);
    if (!input_stream.good()) {
        std::cerr << "Can not open " << filename_ << std::endl;
        return;
    } 
    
    std::getline(input_stream, input_, '\0');

    n_line_ = 1;
    for (;;) {
        n_line_ += skipWhiteSpace();
        if (input_[index_] == '#')
            skipComment();
        if (input_[index_] == 'v') {
            if (expectKeyword("v")) {
                geom_add_vertex(global_vertices);
            } else if (expectKeyword("vt")) {
                geom_add_uv_vertex(global_vertices);
            } else if (expectKeyword("vn")) {
                geom_add_vertex_normal(global_vertices);
            }
        }
        if (input_[index_] == 'f') {
            if (expectKeyword("f")) {
                geom_add_ploygon(geometry, global_vertices);
            }
        }
    }
}

#define parse_floats(p, count)                                                          \
    for (int i = 0; i < count; ++i) {                                                   \
        tryParseFloat(p[i]);                                                            \
    }                                                                                   \

void OBJParser::geom_add_vertex(GlobalVertices &global_vertices) {
    glm::vec3 vert;
    parse_floats(glm::value_ptr(vert), 3);
    global_vertices.vertices.push_back(vert);

    // If there is newline after xyz, parse finished.
    // Otherwise, parse rgb data.
    if (auto new_lines = skipWhiteSpace()) {
        n_line_ += new_lines;
        return;
    }

}

void OBJParser::geom_add_vertex_normal(GlobalVertices &global_vertices) {
    glm::vec3 normal;
    parse_floats(glm::value_ptr(normal), 3);
    glm::normalize(normal);
    global_vertices.vertex_normals.push_back(normal);
}

void OBJParser::geom_add_uv_vertex(GlobalVertices &global_vertices) {
    glm::vec2 uv;
    parse_floats(glm::value_ptr(uv), 2);
    global_vertices.uv_vertices.push_back(uv);
}

void OBJParser::geom_add_ploygon(Geometry &geom, GlobalVertices &global_vertices) {
    PolyElem curr_face;
    const int orig_corners_size = geom.face_corners_.size();
    curr_face.start_index_ = orig_corners_size;


    bool face_valid = true;
    /* Parse until new line*/
    while (skipWhiteSpace() == 0) {
        PolyCorner corner;
        bool got_uv = false, got_normal = false;
        tryParseInt(corner.vert_index);
        if (input_[index_] == '/') {
            ++index_;
            /* UV index */
            if (input_[index_] != '/') {
                tryParseInt(corner.uv_vert_index);
                got_uv = true;
            }
            /* normal index */
            if (input_[index_] == '/') {
                ++index_;
                tryParseInt(corner.vertex_normal_index);
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
            CHECK_LE(corner.vertex_normal_index, global_vertices.uv_vertices.size());
        }
        geom.face_corners_.push_back(corner);
        curr_face.corner_count_++;

    }

    geom.face_elements_.push_back(curr_face);
    
}