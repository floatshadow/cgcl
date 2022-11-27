
#include "shader.h"
#include <filesystem>
#include <sstream>
#include <exception>
#include <stdexcept>


void Shader::read_and_compile(GLenum shader_type, const char *file_path) {
    std::string shader_code;
    std::ifstream shader_file;
    // throw exceptions.
    shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try  {
#ifdef USE_BUILTIN_SHADER
        std::filesystem::path shader_dir = SHADER_DIR;
        std::filesystem::current_path(shader_dir);
        shader_file.open(file_path);
#else 
        shader_file.open(file_path);
#endif
        std::stringstream ss;
        ss << shader_file.rdbuf();
        shader_file.close();
        shader_code = ss.str();
    }
    catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    /* compile and check */
    int success;
    char infoLog[512];
    const char *source_code = shader_code.c_str();

    ID_ = glCreateShader(shader_type);
    glShaderSource(ID_, 1, &source_code, NULL);
    glCompileShader(ID_);
    // 打印编译错误（如果有的话）
    glGetShaderiv(ID_, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(ID_, 512, NULL, infoLog);
        std::cout << file_path << ": ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
}

void Shader::delete_shader() {
    glDeleteShader(ID_);
}

void ShaderProgram::create_program() {
    ID_ = glCreateProgram();
}

void ShaderProgram::attach_shader(unsigned int shader_id) {
    glAttachShader(ID_, shader_id);
}

void ShaderProgram::safe_link() {
    int success;
    char infoLog[512];
    glLinkProgram(ID_);
    // 打印连接错误（如果有的话）
    glGetProgramiv(ID_, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID_, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("Shader Link Error");
    }
}

void ShaderProgram::delete_program() {
    glDeleteProgram(ID_);
}
