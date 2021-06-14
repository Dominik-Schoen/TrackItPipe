#pragma once

// Include modules
#include "lib3mf_implicit.hpp"
#include "OptiTrackPoint.hpp"
#include "SteamVRTrackPoint.hpp"
#include "ActionPoint.hpp"
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
  // Is currently a project opened
  bool isProjectOpen();
  // Close the current project
  void closeProject();
  // Is current project modified
  bool isModified();
  // Set project modification status
  void projectModified();
  // UNIVERSAL
  // Get trackpoint
  TrackPoint* getTrackPointById(int id, ActiveTrackingSystem activeTrackingSystem);
  // Add trackpoint
  void addTrackPoint(osg::Vec3 point, osg::Vec3 normal, ActiveTrackingSystem activeTrackingSystem);
  // Count trackpoints
  int getCount(ActiveTrackingSystem activeTrackingSystem);
  // Remove trackpoint
  void removeTrackPoint(int id, ActiveTrackingSystem activeTrackingSystem);
  // NORMAL MODIFIER
  // Update normal modifier
  void updateNormalModifier(osg::Vec3 modifier);
  // Return normal modifier
  osg::Vec3 getNormalModifier();
  // OPTITRACK
  // Get all OptiTrack points
  std::vector<OptiTrackPoint*> getOptiTrackPoints();
  // Update actual OptiTrack settings
  void updateOptiTrackSettings(OptiTrackSettings optiTrackSettings);
  // Return OptiTrack settings
  OptiTrackSettings getOptiTrackSettings();
  // STEAM VR TRACK
  // Get all SteamVRTrack points
  std::vector<SteamVRTrackPoint*> getSteamVRTrackPoints();
  // Update actual SteamVRTrack settings
  void updateSteamVRTrackSettings(SteamVRTrackSettings steamVrTrackSettings);
  // Return SteamVRTrack settings
  SteamVRTrackSettings getSteamVRTrackSettings();
  // ACTION POINTS
  // Get all Action points
  std::vector<ActionPoint*> getActionPoints();
  // Update actual Action point settings
  void updateActionPointSettings(ActionPointSettings actionPointSettings);
  // Return Action point settings
  ActionPointSettings getActionPointSettings();
  // Check if an identifier is already in use
  unsigned int actionPointIdentifierInUse(std::string candidate, int current);

private:
  bool _projectLoaded;
  bool _projectModified;
  Lib3MF::PWrapper _wrapper;
  Lib3MF::PModel _project;
  std::string _projectFile;
  std::vector<OptiTrackPoint*> _optiTrackPoints;
  std::vector<SteamVRTrackPoint*> _steamVrTrackPoints;
  std::vector<ActionPoint*> _actionPoints;
  OptiTrackSettings _optiTrackSettings = OptiTrackSettings {OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS};
  EMFTrackSettings _emfTrackSettings;
  SteamVRTrackSettings _steamVrTrackSettings = SteamVRTrackSettings {STEAMVR_DEFAULT_LENGTH};
  ActionPointSettings _actionPointSettings = ActionPointSettings {ACTIONPOINT_DEFAULT_IDENFIFIER};
  osg::Vec3 _normalModifier = osg::Vec3(0.0f, 0.0f, 0.0f);
  void load3mfLib();
  void reset();
  void render3MFMesh();
  void updateMetaData();
  void loadMetaData();
  std::vector<float> osgVecToStdVec(osg::Vec3f input);
  osg::Vec3f stdVecToOsgVec(std::vector<float> input);
};
