#pragma once

#include "lib3mf_implicit.hpp"
#include "OptiTrackPoint.hpp"

#include <string>

typedef struct {
  bool OptiTrack;
  bool EMFTrack;
  bool SteamVRTrack;
} ExportSettings;

class ProjectStore {
public:
  // Create an empty project
  ProjectStore();
  // Destructor
  ~ProjectStore();
  // Load a mesh
  void loadMesh(std::string meshFile);
  // Load a project
  bool loadProject(std::string projectFile);
  // Save the project to the loaded file
  bool saveProject();
  // Save the project to a new file
  bool saveProject(std::string projectFile);
  // Export the project
  bool exportProject(std::string path, ExportSettings settings);

private:
  bool projectLoaded;
  Lib3MF::PWrapper _wrapper;
  Lib3MF::PModel _project;
  std::string _projectFile;
  std::vector<OptiTrackPoint> _optiTrackPoints;
  void load3mfLib();
  void render3MFMesh(const std::vector<Lib3MF::sPosition> verticesBuffer, const std::vector<Lib3MF::sTriangle> triangleBuffer);
  void exportMetaData();
};
