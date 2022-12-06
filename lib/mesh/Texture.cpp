#include "cgcl/mesh/Texture.h"

#include "cgcl/utils/Loader.h"
#include "cgcl/utils/logging.h"

#include "glad/glad.h"
#include "stb/stb_image.h"
using namespace cgcl;


constexpr GLuint DEFAULT_TEXTURE_WRAP = GL_REPEAT;
constexpr GLuint DEFAULT_TEXTURE_FILTER = GL_LINEAR;


Texture::~Texture() {
    if (texture_id_ != -1) {
        glDeleteTextures(1, &texture_id_);
    }
}

void Texture::LoadTexture(const std::string &file_path) {
    if (texture_id_ != -1) {
        LOG(WARNING) << "Already loaded texture ID: " << texture_id_;
        glDeleteTextures(1, &texture_id_);
    }
    unsigned int texture;
    glGenBuffers(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DEFAULT_TEXTURE_WRAP);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DEFAULT_TEXTURE_WRAP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DEFAULT_TEXTURE_FILTER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_TEXTURE_FILTER);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(
        file_path.c_str(),
        &width, &height, &channels, 0
    );
    CHECK(data) << "Failed to load texture: " << file_path;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    LOG(INFO) << "Load texure: " << file_path 
            << " height: " << height << " width: " << width;
    /// \attention: auto generate mipmap 
    /// as we set mipmap level in glTexImage2D is zero (basic level).
    glGenerateMipmap(GL_TEXTURE_2D);

    texture_id_ = texture; // store texure id.
    stbi_image_free(data);
}

void Texture::BindTexture() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    // LOG(INFO) << "Bind texture ID: " <<  texture_id_;
}