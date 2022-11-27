#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cgcl/surface/WavefrontOBJ.h"
#include "cgcl/mesh/Mesh.h"

#include <iostream>
#include <cmath>
#include <math.h>

#include "shader.h"
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
    Shader vertex, fragment;
    vertex.read_and_compile(GL_VERTEX_SHADER, "vertex.glsl");
    fragment.read_and_compile(GL_FRAGMENT_SHADER, "frag.glsl");

    ShaderProgram program;
    program.create_program();
    program.attach_shader(vertex.ID_);
    program.attach_shader(fragment.ID_);
    program.safe_link();
    vertex.delete_shader();
    fragment.delete_shader();

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


    auto mesh_ptr = cgcl::TriMesh::from_obj("car.obj");
    mesh_ptr->initGL();


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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(program.ID_);

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
        unsigned int view_loc = glGetUniformLocation(program.ID_, "view");
        unsigned int projection_loc = glGetUniformLocation(program.ID_, "projection");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

        unsigned int model_loc = glGetUniformLocation(program.ID_, "model");
        unsigned int object_color_loc = glGetUniformLocation(program.ID_, "object_color");

        glm::mat4 sun_model = Sun.update_model(glm::mat4(1.0f));
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(sun_model));
        glUniform3fv(object_color_loc, 1, Sun.getColor());
        Sun.bind_and_draw();
        Sun.unbind();

        glm::mat4 earth_model =  Earth.update_model(glm::mat4(1.0f));
        float angle = Earth.getAngle();
        // std::cout << glm::to_string(model) << std::endl;
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(earth_model));
        glUniform3fv(object_color_loc, 1, Earth.getColor());
        Earth.bind_and_draw();
        Earth.unbind();

        glm::mat4 venus_model =  Venus.update_model(glm::mat4(1.0f));
        // std::cout << glm::to_string(model) << std::endl;
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(venus_model));
        glUniform3fv(object_color_loc, 1, Venus.getColor());
        Venus.bind_and_draw();
        Venus.unbind();

        glm::mat4 moon_model =  Moon.update_model(earth_model, angle);
        // std::cout << glm::to_string(model) << std::endl;
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(moon_model));
        glUniform3fv(object_color_loc, 1, Moon.getColor());
        Moon.bind_and_draw();
        Moon.unbind();

        
        glm::mat4 car_model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, 0.0f)) ;
        glm::vec3 car_color(1.0f, 0.0f, 0.0f);
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(car_model));
        glUniform3fv(object_color_loc, 1, glm::value_ptr(car_color));
        mesh_ptr->render();
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }


    glfwTerminate();
    return 0;
}