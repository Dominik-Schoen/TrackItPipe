#pragma once

// Include dependencies
#include <string>
#include "lib3mf_implicit.hpp"

class STLImport {
public:
  static void readSTL(std::string file, std::vector<Lib3MF::sPosition>* verticesBuffer, std::vector<Lib3MF::sTriangle>* triangleBuffer);

private:
  static int findVertex(float x, float y, float z, std::vector<Lib3MF::sPosition>* verticesBuffer);
};
