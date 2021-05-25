#pragma once

#include "lib3mf_implicit.hpp"

#include <string>

class ProjectStore {
public:
  // Create an empty project
  ProjectStore();
  // Load an existing project
  ProjectStore(std::string projectFile);
  // Destructor
  ~ProjectStore();
  // Load a mesh
  void loadMesh(std::string meshFile);

private:
  bool projectLoaded;
  Lib3MF::PWrapper _wrapper;
  Lib3MF::PModel _project;
  void load3mfLib();
  void render3MFMesh(const std::vector<Lib3MF::sPosition> verticesBuffer, const std::vector<Lib3MF::sTriangle> triangleBuffer);
};
