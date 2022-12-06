#include "cgcl/mesh/SkyBoxTexture.h"
#include "cgcl/mesh/SkyBoxMesh.h"
#include "cgcl/utils/Loader.h"
#include "cgcl/utils/logging.h"

#include "stb/stb_image.h"
#include "glad/glad.h"
using namespace cgcl;




void 
SkyBoxTexture::LoadTexture(const std::vector<std::string> &box_faces) {
    CHECK_EQ(box_faces.size(), 6UL) 
        << "Expect a 6 face skybox, but get " << box_faces.size() << " texture.";
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    /* read images */
    int width, height, channel;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned i = 0; i < box_faces.size(); ++i) {
        unsigned char *data = stbi_load(
            box_faces[i].c_str(), 
            &width, &height, &channel, 0
        );
        
        CHECK(data) << "Failed to load texture: " << box_faces[i];
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
        );
        LOG(INFO) << "Load texure: " << box_faces[i] 
            << " height: " << height << " width: " << width;
    }
  

    texture_id_ = texture; 
}


void SkyBoxTexture::BindTexture() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
}