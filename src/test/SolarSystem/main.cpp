#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cgcl/surface/WavefrontOBJ.h"
#include "cgcl/mesh/Mesh.h"
#include "cgcl/surface/Bezier.h"
#include "cgcl/utils/Loader.h"
#include "cgcl/platform/OpenGL/GLShader.h"
#include "cgcl/mesh/PhongMaterial.h"

#include <iostream>
#include <cmath>
#include <math.h>

#include "body.h"

// camera argument; World coordinate as unit.
glm::vec3 sky_vector(0.0f, 1.0f, 0.0f);
glm::vec3 e(0.0f, 0.0f, 50.0f); // position vector
glm::vec3 g(0.0f, 0.0f, -1.0f); // view direction
glm::vec3 t; // up vector
GLfloat cx, cy;
int width, height;

/// \brief Callback function for resize the viewport 
/// when user resize the window.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

float current_frame, last_frame, delta_time;

void key_callback(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    /* Camera moving */ 
    float camera_speed = 2.5 * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        e += camera_speed * g;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        e -= camera_speed * g;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        e -= camera_speed * glm::cross(g, sky_vector);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        e += camera_speed * glm::cross(g, sky_vector);
}


float last_x, last_y;
bool first_in = true;
float yaw = -90.0f, pitch = 0.0f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (first_in) {
        last_x = xpos;
        last_y = ypos;
        first_in = false;
    }
    float x_offset = xpos - last_x;
    float y_offset = ypos - last_y;
    last_x = xpos;
    last_y = ypos;

    float sensitivity = 0.05f;
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if (pitch > 89.0f) pitch = 89.0;
    if (pitch < -89.0f) pitch = -89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    g = glm::normalize(front);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "SolarSystem", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Load OpenGL function pointers. */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    /* Load and Compile shaders */

    cgcl::GLShader program(
        cgcl::Loader::readFromRelative("shader/bling-phong/vertex.glsl"),
        cgcl::Loader::readFromRelative("shader/bling-phong/frag.glsl")
    );

    /* Wireframe Mode */
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Body Sun(glm::vec3(0.0f, 0.0f, -10.0f), 10, 0, glm::vec3(1.0f, 0.5f,0.2f));

    Body Earth(glm::vec3(25.0, 0.0f, -10.0), 5, 1.0,glm::vec3(0.2f, 0.2f, 1.0f),
                glm::vec3(0.0f, 0.0f, -10.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
    Body Venus(glm::vec3(-5.0f, 15.0f, -10.0f), 3, 2.0, glm::vec3(1.0f, 0.84f, 0.5f),
                glm::vec3(0.0f, 0.0f, -10.0f),
                glm::vec3(sqrt(3) / 3 , sqrt(3) / 3, sqrt(3) / 3));

    Body Moon(glm::vec3(25.0, 0.0f, 0.0f), 1, 0.4, glm::vec3(0.5f, 0.5f, 0.5f),
                glm::vec3(25.0f, 0.0f, -10.0f),
            glm::vec3(1.0f, 0.0f, 0.0f));

    /* Set up point light source */
    program.Bind();
    program.updateUniformFloat3("light.pos", glm::vec3(0.0f, 0.0f, 5.0f));
    program.updateUniformFloat3("light.Ia", glm::vec3(0.2f, 0.2f, 0.2f));
    program.updateUniformFloat3("light.Id", glm::vec3(0.5f, 0.5f, 0.5f)); 
    program.updateUniformFloat3("light.Is", glm::vec3(1.0f, 1.0f, 1.0f));
    /* Set up body material */
    cgcl::PhongMaterial sun_material(glm::vec3(1.0f, 0.5f,0.2f));
    cgcl::PhongMaterial earth_material(glm::vec3(0.2f, 0.2f, 1.0f));
    cgcl::PhongMaterial venus_material(glm::vec3(1.0f, 0.84f, 0.5f));
    cgcl::PhongMaterial moon_material(glm::vec3(0.5f, 0.5f, 0.5f));
    cgcl::PhongMaterial car_material(glm::vec3(1.0f, 0.0f, 0.0f));

    auto mesh_ptr = cgcl::TriMesh::from_obj("car.obj");
    mesh_ptr->initGL();

    std::vector<std::unique_ptr<cgcl::TriMesh>> car;
    FILE *file = fopen("car.txt", "r");
    assert(file != nullptr);
    unsigned int n_bezier, u, v;
    fscanf(file, "%d", &n_bezier);
    for (int k = 0; k < n_bezier; ++k) {
        fscanf(file, "%d%d", &u, &v);
        std::vector<glm::vec3> ctrl_pts;
        glm::vec3 pos;
        for (int i = 0; i < u; ++i) {
            for (int j = 0; j < v; ++j) {
                fscanf(file, "%f%f%f", &pos.x, &pos.y, &pos.z);
                ctrl_pts.push_back(pos);
            }
        }
        auto mesh = cgcl::TriMesh::from_bezier(cgcl::BezierSurface{u, v, ctrl_pts});
        mesh->initGL();
        car.emplace_back(std::move(mesh));
    }

    glEnable(GL_DEPTH_TEST); // Z buffer depth test.
    // glEnable(GL_LIGHT0);
    /* Render Loop */
    while(!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &width, &height);
        current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        key_callback(window);
        /* Render background color */
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        program.Bind();
        program.updateUniformFloat3("view_pos", e);
        // /* Global transform from world coord -> camera coord -> viewport */
        // /* This tansfrom will as a uniform attribute and utilize the parallelism of GPU */

        /* Viewport translation */
        glm::mat4 view = glm::lookAt(e, e+g, sky_vector);
        
        // transform = view_translate * transform;
        /* Projective viewing */
        // glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1000.0f, 1000.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 
                                                0.1f, 100.0f);

        /* Set view and projection matrix */


        program.updateUniformMat4("view", view);
        program.updateUniformMat4("projection", projection);


        glm::mat4 sun_model = Sun.update_model(glm::mat4(1.0f));
        program.updateUniformMat4("model", sun_model);
        sun_material.updateBareMaterial(program);
        //program.updateUniformFloat3v("object_color", 1, Sun.getColor());
        Sun.bind_and_draw();
        Sun.unbind();

        glm::mat4 earth_model =  Earth.update_model(glm::mat4(1.0f));
        float angle = Earth.getAngle();
        // std::cout << glm::to_string(model) << std::endl;
        program.updateUniformMat4("model", earth_model);
        earth_material.updateBareMaterial(program);
        //program.updateUniformFloat3v("object_color", 1, Earth.getColor());
        Earth.bind_and_draw();
        Earth.unbind();

        glm::mat4 venus_model =  Venus.update_model(glm::mat4(1.0f));
        // std::cout << glm::to_string(model) << std::endl;
        program.updateUniformMat4("model", venus_model);
        venus_material.updateBareMaterial(program);
        //program.updateUniformFloat3v("object_color", 1, Venus.getColor());
        Venus.bind_and_draw();
        Venus.unbind();

        glm::mat4 moon_model =  Moon.update_model(earth_model, angle);
        // std::cout << glm::to_string(model) << std::endl;
        program.updateUniformMat4("model", moon_model);
        moon_material.updateBareMaterial(program);
        //program.updateUniformFloat3v("object_color", 1, Moon.getColor());
        Moon.bind_and_draw();
        Moon.unbind();


        glm::mat4 car_model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, 0.0f)) ;
        car_material.updateBareMaterial(program);
        program.updateUniformMat4("model", car_model);
        //program.updateUniformFloat3("object_color", car_color);
        mesh_ptr->render();

        glm::mat4 bezier_car_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)) ;
        program.updateUniformMat4("model", bezier_car_model);
        // program.updateUniformFloat3("object_color", bezier_car_color);
        for (const auto &mesh : car) {
            mesh->render();
        }
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }


    glfwTerminate();
    return 0;
}