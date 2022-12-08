#include "cgcl/surface/WavefrontOBJ.h"
#include "cgcl/utils/logging.h"

using namespace cgcl;

int main(int argc, char **argv) {
    std::vector<std::unique_ptr<Geometry>> geom;
    GlobalVertices verteces;
    OBJParser importer(argv[1]);
    importer.parse(geom, verteces);

    CHECK_EQ(geom[0]->geometry_name_, "car");
}