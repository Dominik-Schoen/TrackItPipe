#pragma once

// Include modules
#include "lib3mf_implicit.hpp"
#include "OptiTrackPoint.hpp"
#include "TrackSystemSettingsStructs.hpp"
#include "OpenScadRenderer.hpp"
#include "enums.hpp"

// Include dependencies
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
  // UNIVERSAL TRACKPOINT GETTER
  TrackPoint* getTrackPointById(int id, ActiveTrackingSystem activeTrackingSystem);
  // NORMAL MODIFIER
  // Update normal modifier
  void updateNormalModifier(osg::Vec3 modifier);
  // Return normal modifier
  osg::Vec3 getNormalModifier();
  // OPTITRACK
  // Get all OptiTrack points
  std::vector<OptiTrackPoint*> getOptiTrackPoints();
  // Add an OptiTrack point
  void addOptiTrackPoint(osg::Vec3 point, osg::Vec3 normal);
  // Remove an OptiTrack point
  void removeOptiTrackPoint();
  // Update actual OptiTrack settings
  void updateOptiTrackSettings(OptiTrackSettings optiTrackSettings);
  // Return OptiTrack settings
  OptiTrackSettings getOptiTrackSettings();

private:
  bool _projectLoaded;
  bool _projectModified;
  Lib3MF::PWrapper _wrapper;
  Lib3MF::PModel _project;
  std::string _projectFile;
  std::vector<OptiTrackPoint*> _optiTrackPoints;
  OptiTrackSettings _optiTrackSettings = OptiTrackSettings {OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS};
  EMFTrackSettings _emfTrackSettings;
  SteamVRTrackSettings _steamVrTrackSettings;
  osg::Vec3 _normalModifier = osg::Vec3(0.0f, 0.0f, 0.0f);
  void load3mfLib();
  void render3MFMesh();
  void updateMetaData();
  void loadMetaData();
  std::vector<float> osgVecToStdVec(osg::Vec3f input);
  osg::Vec3f stdVecToOsgVec(std::vector<float> input);
};
