#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>



struct Shader {
    unsigned int ID_;
    void read_and_compile(GLenum shader_type, const char *file_path);
    void delete_shader();
};

struct ShaderProgram {
    unsigned int ID_;
    void create_program();
    void attach_shader(unsigned int shader_id);
    void safe_link();
    void delete_program();
};

#endif // SHADER_H