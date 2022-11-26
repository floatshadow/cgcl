#include "cgcl/surface/WavefrontOBJ.h"
#include "cgcl/utils/logging.h"

using namespace cgcl;

int main(int argc, char **argv) {
    Geometry geom;
    GlobalVertices verteces;
    OBJParser importer(argv[1]);
    importer.parse(geom, verteces);

    CHECK_EQ(geom.geometry_name_, "car");
}