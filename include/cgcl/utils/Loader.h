#pragma once

#include <string>

namespace cgcl {


/// \brief When the project is compiled, it will generate 
/// the absolute path of assets directory, and concate your relative path.
class Loader {
public:
    static std::string getAssetPath(const std::string &relative_path);
    static std::string readFromRelative(const std::string &relative_path);
    static std::string readFromAbsolute(const std::string &absolute_path);
};

} // end namespace cgcl