#ifndef BODY_H_
#define BODY_H_

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

/// \file body.h
/// \brief Body Class for star, planet and satellite.

class Body {
public:
    Body() = delete;
    Body(const glm::vec3 &center, float radis, float speed, const glm::vec3 &color,
         const glm::vec3 &orbit_center = glm::vec3(0.0f, 0.0f, 0.0f),
         const glm::vec3 &axis = glm::vec3(0.0f, 0.0f, 0.0f));
    ~Body();
public:
    const GLfloat *getColor() const;
    float getAngle();
    glm::mat4 update_model(glm::mat4 base_trans, float angle = 0.0f);
    void bind_and_draw();
    void unbind();
#ifndef NDEBUG
    void debug(glm::mat4 transform);
#endif

protected:
    void InitBufferData();
    void InitGLObjects();
private:
    glm::vec3 color_;
    glm::vec3 center_;
    float radis_;
    float speed_;
    glm::vec3 orbit_center_;
    glm::vec3 axis_;

    unsigned int VAO, VBO, EBO;
    static const unsigned int N = 64; // latitude and longitude split.
    unsigned int n_vertex_;
    unsigned int n_triangles_;
    bool need_orbit_ = false;
    bool need_rotation_ = false;
    std::vector<float> vertex_;
    std::vector<unsigned int> indices_;
};

#endif // BODY_H_