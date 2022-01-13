#pragma once

// Include modules
#include "lib3mf_implicit.hpp"
#include "OptiTrackPoint.hpp"
#include "EMFTrackPoint.hpp"
#include "SteamVRTrackPoint.hpp"
#include "ActionPoint.hpp"
#include "TrackSystemSettingsStructs.hpp"
#include "OpenScadRenderer.hpp"
#include "enums.hpp"

// Include dependencies
#include <string>
#include <nlohmann/json.hpp>
#include <QThread>

using json = nlohmann::json;

class ProjectStore: public QObject {
  Q_OBJECT

public:
  // Create an empty project
  ProjectStore(QObject* parent = nullptr);
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
  // Is a rendering in progress
  bool isRendering();
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
  // Update normal rotation
  void updateNormalRotation(float normalRotation);
  // Return normal rotation
  float getNormalRotation();
  // Update compensation
  void updateCompensation(bool compensation);
  // Return compensation
  bool getCompensation();
  // Get the minimum required trackpoint count for a given tracking system
  int getMinimumRequiredPoints(ActiveTrackingSystem activeTrackingSystem);
  // OPTITRACK
  // Get all OptiTrack points
  std::vector<OptiTrackPoint*> getOptiTrackPoints();
  // Update actual OptiTrack settings
  void updateOptiTrackSettings(OptiTrackSettings optiTrackSettings);
  // Return OptiTrack settings
  OptiTrackSettings getOptiTrackSettings();
  // EMF TRACK
  // Get all EMFTrack points
  std::vector<EMFTrackPoint*> getEMFTrackPoints();
  // Update actual EMFTrack settings
  void updateEMFTrackSettings(EMFTrackSettings emfTrackSettings);
  // Return EMFTrack settings
  EMFTrackSettings getEMFTrackSettings();
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

private slots:
  void renderThreadDone();

private:
  bool _projectLoaded;
  bool _projectModified;
  Lib3MF::PWrapper _wrapper;
  Lib3MF::PModel _project;
  std::string _projectFile;
  std::vector<OptiTrackPoint*> _optiTrackPoints;
  std::vector<EMFTrackPoint*> _emfTrackPoints;
  std::vector<SteamVRTrackPoint*> _steamVrTrackPoints;
  std::vector<ActionPoint*> _actionPoints;
  OptiTrackSettings _optiTrackSettings = OptiTrackSettings {OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS};
  EMFTrackSettings _emfTrackSettings = EMFTrackSettings {EMFTRACK_DEFAULT_WIDTH, EMFTRACK_DEFAULT_HEIGHT, EMFTRACK_DEFAULT_DEPTH};
  SteamVRTrackSettings _steamVrTrackSettings = SteamVRTrackSettings {STEAMVR_DEFAULT_LENGTH};
  ActionPointSettings _actionPointSettings = ActionPointSettings {ACTIONPOINT_DEFAULT_IDENFIFIER};
  osg::Vec3 _normalModifier = osg::Vec3(0.0f, 0.0f, 0.0f);
  float _normalRotation = 0.0f;
  bool _compensation = true;
  QThread* _optiTrackRenderThread;
  QThread* _emfTrackRenderThread;
  QThread* _steamVrTrackRenderThread;
  bool _exportRunning = false;
  ExportSettings _currentExportSettings;
  int _wantedExports;
  int _doneExports;
  std::string _exportPath;
  void load3mfLib();
  void reset();
  void render3MFMesh();
  void updateMetaData();
  void loadMetaData();
  static void renderOptiTrackInThread(std::vector<OptiTrackPoint*> optiTrackPoints);
  static void renderEMFTrackInThread(std::vector<EMFTrackPoint*> emfTrackPoints);
  static void renderSteamVRTrackInThread(std::vector<SteamVRTrackPoint*> steamVrTrackPoints);
  std::vector<float> osgVecToStdVec(osg::Vec3f input);
  osg::Vec3f stdVecToOsgVec(std::vector<float> input);
};
