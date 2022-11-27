#include "body.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <ostream>


Body::Body(const glm::vec3 &center, float radis, float speed, const glm::vec3 &color,
           const glm::vec3 &orbit_center,
           const glm::vec3 &axis)
    : center_(center), radis_(radis), speed_(speed), color_(color),
    orbit_center_(orbit_center), axis_(axis)
{
    n_vertex_ = N * N;
    n_triangles_ = N * (N-1) * 2;
    vertex_.reserve(n_vertex_ * 3);
    indices_.reserve(n_triangles_ * 3);
    if (glm::length(axis) > 1e-4) 
        need_orbit_ = true; 
    InitBufferData();
    InitGLObjects();
}

Body::~Body() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Body::InitGLObjects() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    /* vertex array object */
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // now operation to GL_ARRAY_BUFFER will sets VBO.
    glBufferData(GL_ARRAY_BUFFER, n_vertex_ * 3 * sizeof(float), vertex_.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_triangles_ * 3 * sizeof(unsigned int), indices_.data(), GL_DYNAMIC_DRAW);
    /* Link vertex attribute */
    /* VBO is a buffer handler, attribute got its data from VBO which binded to GL_ARRAY_BUFFER */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    /* enable position 0 attribute */
    glEnableVertexAttribArray(0);
    
    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

const double PI = acos(-1.0);
void Body::InitBufferData() {

    /* generate vertex */
    double dtheta = 2 * PI / N;
    double dphi = PI / (N + 1);

    /* split vertex */
#pragma parallel for
    for (int i = 0; i < N; ++i) {
        int longitude_index = i * N;
        double theta = i * dtheta;
        for (int j = 0; j < N; ++j) {
            double phi = dphi * (j + 1);
            int vertex_index = (longitude_index + j) * 3;
            vertex_[vertex_index    ] = radis_ * sin(phi) * cos(theta); // x
            vertex_[vertex_index + 1] = radis_ * cos(phi); // y
            vertex_[vertex_index + 2] = -radis_ * sin(phi) * sin(theta); // z
        }
    }
#pragma parallel for
    for (int i = 0; i < n_vertex_; ++i) {
        vertex_[i * 3] += center_.x;
        vertex_[i * 3 + 1] += center_.y;
        vertex_[i * 3 + 2] += center_.z;
    }

    /* triangle split sphere */
#pragma parallel for 
    for (int i = 0; i < N; ++i) {
        int longitude_index = i * N;
        for (int j = 0; j < N-1; ++j) {
            int vertex_index = longitude_index + j;
            int triangle_index = ((i * (N-1) + j) * 2) * 3;
            // std::cout << vertex_index << " " << triangle_index << std::endl;
            indices_[triangle_index] = vertex_index;
            indices_[triangle_index + 1] = vertex_index + 1;
            indices_[triangle_index + 2] = (vertex_index + N) % n_vertex_;

            indices_[triangle_index + 3] = vertex_index + 1;
            indices_[triangle_index + 4] = (vertex_index + N) % n_vertex_;
            indices_[triangle_index + 5] = (vertex_index + (N+1)) % n_vertex_;
        }
    }
    /* modify buffer data */
    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO); // now operation to GL_ARRAY_BUFFER will sets VBO.
    // glBufferSubData(GL_ARRAY_BUFFER, 0, n_vertex_ * 3 * sizeof(float), vertex_.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, n_triangles_ * 3 * sizeof(unsigned int), indices_.data());

}

const GLfloat *Body::getColor() const { return glm::value_ptr(color_); }

float Body::getAngle() { return (float)glfwGetTime() * speed_; }


glm::mat4 Body::update_model(glm::mat4 base_trans, float angle) {
    /* rotate the center of the body */
    glm::mat4 model = base_trans;
    if (need_orbit_) {
        glm::vec3 new_orbit_center = base_trans * glm::vec4(orbit_center_, 1.0f);
        glm::vec3 new_axis = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(axis_, 1.0f);
        
        glm::mat4 rev_trans = glm::translate(glm::mat4(1.0f), -new_orbit_center);
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), 
                                       (float)glfwGetTime() * speed_, new_axis); 
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), new_orbit_center);
        model = trans * rotate * rev_trans * base_trans;
    }
    return model;
}

void Body::bind_and_draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, n_triangles_ * 3, GL_UNSIGNED_INT, 0);
}

void Body::unbind() {
    glBindVertexArray(0);
}

#ifndef NDEBUG
void Body::debug(glm::mat4 transform) {
    std::cout << n_vertex_ << " " << n_triangles_ << std::endl;
    glm::vec4 test = glm::vec4(vertex_[0], vertex_[1], vertex_[2], 1.0f);
    std::cout << test.x << " " << test.y << " " << test.z << " " << test.w << std::endl;
    test = transform * test;
    std::cout << test.x << " " << test.y << " " << test.z << " " << test.w << std::endl;
    for (int i = 0; i < n_triangles_; ++i) {
        std::cout << indices_[i*3] << " " << indices_[i*3+1] << " " << indices_[i*3+2] << std::endl;
    }
    for (int i = 0; i < n_vertex_; ++i) {
        std::cout << vertex_[i*3] << " " << vertex_[i*3+1] << " " << vertex_[i*3+2] << std::endl;
    }
}
#endif
