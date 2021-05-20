#pragma once

#include <QString>
#include "lib3mf_implicit.hpp"

class ProjectStore {
public:
  // Create an empty project
  ProjectStore();
  // Load an existing project
  ProjectStore(QString projectFile);
  // Load a mesh
  void loadMesh(QString meshFileString);

private:
  bool projectLoaded;
  Lib3MF::PWrapper _wrapper;
  Lib3MF::PModel _project;
  void load3mfLib();
};
