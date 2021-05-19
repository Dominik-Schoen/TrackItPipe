#pragma once

#include <QString>

class ProjectStore {
public:
  // Create an empty project
  ProjectStore();
  // Load an existing project
  ProjectStore(QString projectFile);
  // Load a mesh
  void loadMesh(QString meshFile);

private:
  bool projectLoaded;
};
