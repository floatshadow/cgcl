#include "cgcl/utils/Loader.h"
#include "cgcl/config.h"

#include "cgcl/utils/logging.h"
#include <filesystem>
#include <fstream>

using namespace cgcl;
using namespace std::filesystem;


std::string Loader::getAssetPath(const std::string &relative_path) {
    path assets_dir(utils::project_assets_root_dir);
    CHECK(exists(assets_dir)) << "Failed to open assets directory";

    path assets_file_path = assets_dir.append(relative_path);
    return assets_file_path.string();
}

std::string Loader::getParentPath(const std::string &file_path) {
    path file(file_path);
    CHECK(exists(file)) << "Failed to locate " << file_path;
    if (!file.has_filename()) {
        LOG(WARNING) << "Expect a file path, but find " << file_path;
    }
    return file.parent_path().string();
}

std::string Loader::getFileFromPath(const std::string &filename, const std::string &file_path) {
    path file_path_dir(file_path);
    CHECK(exists(file_path_dir)) << "Failed to locate " << file_path;
    return file_path_dir.append(filename).string();
}

std::string Loader::readFromRelative(const std::string &relative_path) {
    auto absolute_path = getAssetPath(relative_path);
    std::ifstream input_stream(absolute_path);
    std::string input;
    CHECK(input_stream) 
        << "Failed to open " <<  relative_path << " in " << absolute_path;
    std::getline(input_stream, input, '\0');
    return input;
}

std::string Loader::readFromAbsolute(const std::string &absolute_path) {
    std::ifstream input_stream(absolute_path);
    std::string input;
    CHECK(input_stream) 
        << "Failed to open " <<  absolute_path;
    std::getline(input_stream, input, '\0');
    return input; 
}