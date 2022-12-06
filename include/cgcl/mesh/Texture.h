#pragma once

#include <vector>
#include <string>

namespace cgcl {

class Texture {
public:
    unsigned int texture_id_ = -1;

    virtual ~Texture();
    virtual void LoadTexture(const std::string &file_path);
    virtual void BindTexture() const;
};

} // end namespace cgcl