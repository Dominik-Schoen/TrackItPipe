// Include own headers
#include "ProjectStore.hpp"

// Include modules
#include "MainWindow.hpp"
#include "StringBasics.hpp"
#include "TrackPointRenderer.hpp"
#include "PlatformSupport.hpp"
#include "STLImport.hpp"
#include "MeshTools.hpp"

// Include dependencies
#include <typeinfo>
#include <iostream>
#include <filesystem>

#define META_NAMESPACE "tk-ar-tracking"

ProjectStore::ProjectStore(QObject* parent): QObject() {
  _projectLoaded = false;
  _projectModified = false;
  load3mfLib();
}

ProjectStore::~ProjectStore() {
}

void ProjectStore::loadMesh(std::string meshFile) {
  if (meshFile == "") {
    return;
  }
  if (StringBasics::endsWithCaseInsensitive(meshFile, ".STL")) {
    _projectLoaded = true;
    _projectModified = false;
    // Read STL file
    std::vector<Lib3MF::sPosition> verticesBuffer;
    std::vector<Lib3MF::sTriangle> triangleBuffer;
    STLImport::readSTL(meshFile, &verticesBuffer, &triangleBuffer);
    Lib3MF::PMeshObject baseMesh = _project->AddMeshObject();
    baseMesh->SetGeometry(verticesBuffer, triangleBuffer);
    render3MFMesh();
    MainWindow::getInstance()->renderView(Edit);
  } else if (StringBasics::endsWithCaseInsensitive(meshFile, ".3MF")) {
    _projectLoaded = true;
    _projectModified = false;
    // Read 3MF file
    Lib3MF::PReader reader = _project->QueryReader("3mf");
    reader->ReadFromFile(meshFile);
    render3MFMesh();
    MainWindow::getInstance()->renderView(Edit);
  } else {
    MainWindow::getInstance()->showErrorMessage("Unsupported file type. Please use .stl or .3mf files.", "Error opening file.");
  }
}

bool ProjectStore::loadProject(std::string projectFile) {
  if (projectFile == "") {
    return false;
  }
  if (!_projectLoaded) {
    Lib3MF::PReader reader = _project->QueryReader("3mf");
    reader->ReadFromFile(projectFile);
    _projectLoaded = true;
    _projectModified = false;
    _projectFile = projectFile;
    loadMetaData();
    return true;
  }
  return false;
}

bool ProjectStore::saveProject() {
  if (_projectFile != "") {
    return saveProject(_projectFile);
  }
  return false;
}

bool ProjectStore::saveProject(std::string path) {
  if (path == "") {
    return false;
  }
  updateMetaData();
  Lib3MF::PWriter writer = _project->QueryWriter("3mf");
  writer->WriteToFile(path);
  _projectFile = path;
  _projectModified = false;
  return true;
}

bool ProjectStore::exportProject(std::string path, ExportSettings settings) {
  if (path == "") {
    return false;
  }
  MainWindow::getInstance()->getEditWiget()->setExportAvailable(false);
  _exportPath = path;
  // Base for rendering mesh
  Lib3MF::PWriter writer = _project->QueryWriter("3mf");
  writer->WriteToFile(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export.3mf");
  // Store export settings
  _wantedExports = 0;
  _doneExports = 0;
  _currentExportSettings = settings;
  if (settings.OptiTrack) _wantedExports++;
  if (settings.EMFTrack) _wantedExports++;
  if (settings.SteamVRTrack) _wantedExports++;
  MainWindow::getInstance()->getEditWiget()->setExportStatus(_wantedExports, _doneExports);
  if (settings.OptiTrack) {
    _optiTrackRenderThread = QThread::create(renderOptiTrackInThread, _optiTrackPoints);
    _optiTrackRenderThread->setObjectName("TrackpointApp OptiTrack Renderer");
    QObject::connect(_optiTrackRenderThread, &QThread::finished, this, &ProjectStore::renderThreadDone);
    _optiTrackRenderThread->start();
  }
  if (settings.EMFTrack) {
    _emfTrackRenderThread = QThread::create(renderEMFTrackInThread, _emfTrackPoints);
    _emfTrackRenderThread->setObjectName("TrackpointApp EMFTrack Renderer");
    QObject::connect(_emfTrackRenderThread, &QThread::finished, this, &ProjectStore::renderThreadDone);
    _emfTrackRenderThread->start();
  }
  if (settings.SteamVRTrack) {
    _steamVrTrackRenderThread = QThread::create(renderSteamVRTrackInThread, _steamVrTrackPoints);
    _steamVrTrackRenderThread->setObjectName("TrackpointApp SteamVR Track Renderer");
    QObject::connect(_steamVrTrackRenderThread, &QThread::finished, this, &ProjectStore::renderThreadDone);
    _steamVrTrackRenderThread->start();
  }
  _exportRunning = true;
  return true;
}

bool ProjectStore::isProjectOpen() {
  return _projectLoaded;
}

void ProjectStore::closeProject() {
  _projectLoaded = false;
  _projectModified = false;
  reset();
}

bool ProjectStore::isModified() {
  return _projectModified;
}

bool ProjectStore::isRendering() {
  return _exportRunning;
}

void ProjectStore::projectModified() {
  _projectModified = true;
}

TrackPoint* ProjectStore::getTrackPointById(int id, ActiveTrackingSystem activeTrackingSystem) {
  switch(activeTrackingSystem) {
    case OptiTrack: {
      return _optiTrackPoints[id];
    };
    case EMFTrack: {
      return _emfTrackPoints[id];
    };
    case SteamVRTrack: {
      return _steamVrTrackPoints[id];
    };
    case ActionPoints: {
      return _actionPoints[id];
    };
  }
}

void ProjectStore::addTrackPoint(osg::Vec3 point, osg::Vec3 normal, ActiveTrackingSystem activeTrackingSystem) {
  switch(activeTrackingSystem) {
    case OptiTrack: {
      OptiTrackPoint* optiTrackPoint = new OptiTrackPoint(point, normal, _normalModifier, _normalRotation, _compensation, _optiTrackSettings.length, _optiTrackSettings.radius);
      _optiTrackPoints.push_back(optiTrackPoint);
      MeshTools::optiTrackSanityCheck(_optiTrackPoints, false);
      break;
    }
    case EMFTrack: {
      normal = normal.operator*(-1.0f);
      EMFTrackPoint* emfTrackPoint = new EMFTrackPoint(point, normal, _normalModifier, _normalRotation, _compensation, _emfTrackSettings.width, _emfTrackSettings.height, _emfTrackSettings.depth);
      _emfTrackPoints.push_back(emfTrackPoint);
      break;
    }
    case SteamVRTrack: {
      SteamVRTrackPoint* steamVrTrackPoint = new SteamVRTrackPoint(point, normal, _normalModifier, _normalRotation, _compensation, _steamVrTrackSettings.length);
      _steamVrTrackPoints.push_back(steamVrTrackPoint);
      MeshTools::steamVrTrackCollisionCheck(_steamVrTrackPoints, false, MainWindow::getInstance()->getOsgWidget()->getVerifyGroup());
      break;
    }
    case ActionPoints: {
      ActionPoint* actionPoint = new ActionPoint(point, normal, _normalModifier, _normalRotation, _compensation, _actionPointSettings.identifier);
      _actionPoints.push_back(actionPoint);
      break;
    }
  }
  projectModified();
  MainWindow::getInstance()->getEditWiget()->updateTrackpointCount();
}

int ProjectStore::getCount(ActiveTrackingSystem activeTrackingSystem) {
  switch(activeTrackingSystem) {
    case OptiTrack: {
      return _optiTrackPoints.size();
    };
    case EMFTrack: {
      return _emfTrackPoints.size();
    };
    case SteamVRTrack: {
      return _steamVrTrackPoints.size();
    };
    case ActionPoints: {
      return _actionPoints.size();
    };
  }
}

void ProjectStore::removeTrackPoint(int id, ActiveTrackingSystem activeTrackingSystem) {
  switch(activeTrackingSystem) {
    case OptiTrack: {
      _optiTrackPoints.erase(_optiTrackPoints.begin() + id);
      break;
    }
    case EMFTrack: {
      _emfTrackPoints.erase(_emfTrackPoints.begin() + id);
      break;
    }
    case SteamVRTrack: {
      _steamVrTrackPoints.erase(_steamVrTrackPoints.begin() + id);
      break;
    }
    case ActionPoints: {
      _actionPoints.erase(_actionPoints.begin() + id);
      break;
    }
  }
  projectModified();
  MainWindow::getInstance()->getEditWiget()->updateTrackpointCount();
}

void ProjectStore::updateNormalModifier(osg::Vec3 modifier) {
  _normalModifier = modifier;
}

osg::Vec3 ProjectStore::getNormalModifier() {
  return _normalModifier;
}

void ProjectStore::updateNormalRotation(float normalRotation) {
  _normalRotation = normalRotation;
}

float ProjectStore::getNormalRotation() {
  return _normalRotation;
}

void ProjectStore::updateCompensation(bool compensation) {
  _compensation = compensation;
}

bool ProjectStore::getCompensation() {
  return _compensation;
}

int ProjectStore::getMinimumRequiredPoints(ActiveTrackingSystem activeTrackingSystem) {
  switch(activeTrackingSystem) {
    case OptiTrack:
      return OptiTrackPoint::getMinimumRequiredPoints();
    case EMFTrack:
      return EMFTrackPoint::getMinimumRequiredPoints();
    case SteamVRTrack:
      return SteamVRTrackPoint::getMinimumRequiredPoints();
    case ActionPoints:
      return 0;
  }
}

std::vector<OptiTrackPoint*> ProjectStore::getOptiTrackPoints() {
  return _optiTrackPoints;
}

void ProjectStore::updateOptiTrackSettings(OptiTrackSettings optiTrackSettings) {
  _optiTrackSettings = optiTrackSettings;
}

OptiTrackSettings ProjectStore::getOptiTrackSettings() {
  return _optiTrackSettings;
}

std::vector<EMFTrackPoint*> ProjectStore::getEMFTrackPoints() {
  return _emfTrackPoints;
}

void ProjectStore::updateEMFTrackSettings(EMFTrackSettings emfTrackSettings) {
  _emfTrackSettings = emfTrackSettings;
}

EMFTrackSettings ProjectStore::getEMFTrackSettings() {
  return _emfTrackSettings;
}

std::vector<SteamVRTrackPoint*> ProjectStore::getSteamVRTrackPoints() {
  return _steamVrTrackPoints;
}

void ProjectStore::updateSteamVRTrackSettings(SteamVRTrackSettings steamVrTrackSettings) {
  _steamVrTrackSettings = steamVrTrackSettings;
}

SteamVRTrackSettings ProjectStore::getSteamVRTrackSettings() {
  return _steamVrTrackSettings;
}

std::vector<ActionPoint*> ProjectStore::getActionPoints() {
  return _actionPoints;
}

void ProjectStore::updateActionPointSettings(ActionPointSettings actionPointSettings) {
  _actionPointSettings = actionPointSettings;
}

ActionPointSettings ProjectStore::getActionPointSettings() {
  return _actionPointSettings;
}

unsigned int ProjectStore::actionPointIdentifierInUse(std::string candidate, int current) {
  unsigned int count = 0;
  int i = 0;
  for (ActionPoint* actionPoint: _actionPoints) {
    if (i != current && candidate.compare(actionPoint->getIdentifier()) == 0) {
      count++;
    }
    i++;
  }
  return count;
}

void ProjectStore::renderThreadDone() {
  _doneExports++;
  MainWindow::getInstance()->getEditWiget()->setExportStatus(_wantedExports, _doneExports);
  if (_wantedExports == _doneExports) {
    // Export file
    Lib3MF::PModel exportModel = _wrapper->CreateModel();
    Lib3MF::PMetaDataGroup metaData = exportModel->GetMetaDataGroup();
    if (_currentExportSettings.OptiTrack) {
      Lib3MF::PModel optiTrackModel = _wrapper->CreateModel();
      Lib3MF::PReader reader = optiTrackModel->QueryReader("3mf");
      reader->ReadFromFile(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_render_optitrack.3mf");
      Lib3MF::PMeshObjectIterator meshIterator = optiTrackModel->GetMeshObjects();
      if (meshIterator->Count() == 1) {
        meshIterator->MoveNext();
        Lib3MF::PMeshObject renderedMesh = meshIterator->GetCurrentMeshObject();
        Lib3MF::PMeshObject exportMesh = exportModel->AddMeshObject();
        exportMesh->SetName("optitrack");
        std::vector<Lib3MF::sPosition> verticesBuffer;
        std::vector<Lib3MF::sTriangle> triangleBuffer;
        renderedMesh->GetVertices(verticesBuffer);
        renderedMesh->GetTriangleIndices(triangleBuffer);
        exportMesh->SetGeometry(verticesBuffer, triangleBuffer);
        json trackpointData = json::array();
        for (OptiTrackPoint* point: _optiTrackPoints) {
          osg::Vec3 trackPoint = point->getTrackPoint();
          osg::Vec3 trackNormal = point->getNormal();
          json pointData = {
            {"point", {trackPoint.x(), trackPoint.y(), trackPoint.z()}},
            {"normal", {trackNormal.x(), trackNormal.y(), trackNormal.z()}}
          };
          trackpointData.push_back(pointData);
        }

        Lib3MF::PMetaDataGroup optiMetaData = exportMesh->GetMetaDataGroup();
        optiMetaData->AddMetaData(META_NAMESPACE, "trackpoints-optitrack", trackpointData.dump(), "string", true);
        exportModel->AddBuildItem(exportMesh.get(), _wrapper->GetIdentityTransform());
      } else {
        MainWindow::getInstance()->showErrorMessage("An error occured while rendering OptiTrack model: inconsistent data.", "Error exporting project.");
      }
    }
    if (_currentExportSettings.EMFTrack) {
      Lib3MF::PModel emfTrackModel = _wrapper->CreateModel();
      Lib3MF::PReader reader = emfTrackModel->QueryReader("3mf");
      reader->ReadFromFile(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_render_emftrack.3mf");
      Lib3MF::PMeshObjectIterator meshIterator = emfTrackModel->GetMeshObjects();
      if (meshIterator->Count() == 1) {
        meshIterator->MoveNext();
        Lib3MF::PMeshObject renderedMesh = meshIterator->GetCurrentMeshObject();
        Lib3MF::PMeshObject exportMesh = exportModel->AddMeshObject();
        exportMesh->SetName("emftrack");
        std::vector<Lib3MF::sPosition> verticesBuffer;
        std::vector<Lib3MF::sTriangle> triangleBuffer;
        renderedMesh->GetVertices(verticesBuffer);
        renderedMesh->GetTriangleIndices(triangleBuffer);
        exportMesh->SetGeometry(verticesBuffer, triangleBuffer);
        json trackpointData = json::array();
        for (EMFTrackPoint* point: _emfTrackPoints) {
          osg::Vec3 moveToMid = point->getNormal().operator*(-(point->getDepth() / 2));
          osg::Vec3 trackPoint = point->getTrackPoint().operator+(moveToMid);
          osg::Vec3 trackNormal = point->getNormal();
          json pointData = {
            {"point", {trackPoint.x(), trackPoint.y(), trackPoint.z()}},
            {"normal", {trackNormal.x(), trackNormal.y(), trackNormal.z()}},
            {"rotation", point->getNormalRotation()}
          };
          trackpointData.push_back(pointData);
        }

        Lib3MF::PMetaDataGroup emfMetaData = exportMesh->GetMetaDataGroup();
        emfMetaData->AddMetaData(META_NAMESPACE, "trackpoints-emftrack", trackpointData.dump(), "string", true);
        exportModel->AddBuildItem(exportMesh.get(), _wrapper->GetIdentityTransform());
      } else {
        MainWindow::getInstance()->showErrorMessage("An error occured while rendering EMFTrack model: inconsistent data.", "Error exporting project.");
      }
    }
    if (_currentExportSettings.SteamVRTrack) {
      Lib3MF::PModel steamVrTrackModel = _wrapper->CreateModel();
      Lib3MF::PReader reader = steamVrTrackModel->QueryReader("3mf");
      reader->ReadFromFile(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_render_steamvrtrack.3mf");
      Lib3MF::PMeshObjectIterator meshIterator = steamVrTrackModel->GetMeshObjects();
      if (meshIterator->Count() == 1) {
        meshIterator->MoveNext();
        Lib3MF::PMeshObject renderedMesh = meshIterator->GetCurrentMeshObject();
        Lib3MF::PMeshObject exportMesh = exportModel->AddMeshObject();
        exportMesh->SetName("steamvrtrack");
        std::vector<Lib3MF::sPosition> verticesBuffer;
        std::vector<Lib3MF::sTriangle> triangleBuffer;
        renderedMesh->GetVertices(verticesBuffer);
        renderedMesh->GetTriangleIndices(triangleBuffer);
        exportMesh->SetGeometry(verticesBuffer, triangleBuffer);
        json trackpointData = json::array();
        for (SteamVRTrackPoint* point: _steamVrTrackPoints) {
          osg::Vec3 trackPoint = point->getTrackPoint();
          osg::Vec3 trackNormal = point->getNormal();
          json pointData = {
            {"point", {trackPoint.x(), trackPoint.y(), trackPoint.z()}},
            {"normal", {trackNormal.x(), trackNormal.y(), trackNormal.z()}}
          };
          trackpointData.push_back(pointData);
        }

        Lib3MF::PMetaDataGroup steamVrMetaData = exportMesh->GetMetaDataGroup();
        steamVrMetaData->AddMetaData(META_NAMESPACE, "trackpoints-steamvrtrack", trackpointData.dump(), "string", true);
        exportModel->AddBuildItem(exportMesh.get(), _wrapper->GetIdentityTransform());
      } else {
        MainWindow::getInstance()->showErrorMessage("An error occured while rendering SteamVR Track model: inconsistent data.", "Error exporting project.");
      }
    }
    // Export action point metadata
    json actionPointData;
    for (ActionPoint* point: _actionPoints) {
      osg::Vec3 translation = point->getTranslation();
      osg::Vec3 normal = point->getNormal();
      actionPointData[point->getIdentifier()] = {
        {"point", {translation.x(), translation.y(), translation.z()}},
        {"normal", {normal.x(), normal.y(), normal.z()}}
      };
    }
    metaData->AddMetaData(META_NAMESPACE, "trackpoints-actionpoints", actionPointData.dump(), "string", true);
    Lib3MF::PWriter exportWriter = exportModel->QueryWriter("3mf");
    exportWriter->WriteToFile(_exportPath);
    _exportRunning = false;
    MainWindow::getInstance()->getEditWiget()->setExportAvailable(true);
  }
}

void ProjectStore::load3mfLib() {
  _wrapper = Lib3MF::CWrapper::loadLibrary();
  _project = _wrapper->CreateModel();
}

void ProjectStore::reset() {
  _project = _wrapper->CreateModel();
  _optiTrackPoints.clear();
  _steamVrTrackPoints.clear();
  _actionPoints.clear();
  _optiTrackSettings = OptiTrackSettings {OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS};
  _steamVrTrackSettings = SteamVRTrackSettings {STEAMVR_DEFAULT_LENGTH};
  _actionPointSettings = ActionPointSettings {ACTIONPOINT_DEFAULT_IDENFIFIER};
  _normalModifier = osg::Vec3(0.0f, 0.0f, 0.0f);
}

void ProjectStore::render3MFMesh() {
  // Get meshes
  Lib3MF::PMeshObjectIterator meshIterator = _project->GetMeshObjects();
  // Our use case supports just a single mesh per project
  if (meshIterator->Count() != 1) {
    MainWindow::getInstance()->showErrorMessage("The input file contains more than one mesh. Currently, we don't support this case, so the first mesh will be used.", "Input file error.");
  }
  meshIterator->MoveNext();
  Lib3MF::PMeshObject mesh = meshIterator->GetCurrentMeshObject();
  // Load vertices and triangles and render them
  std::vector<Lib3MF::sPosition> verticesBuffer;
  mesh->GetVertices(verticesBuffer);
  std::vector<Lib3MF::sTriangle> triangleBuffer;
  mesh->GetTriangleIndices(triangleBuffer);

  // Create osg style arrays
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
  // Convert data to osg format and calculate vertex normals
  MeshTools::calculateNormals(verticesBuffer, triangleBuffer, vertices, normals);
  // Call renderer
  MainWindow* mainWindow = MainWindow::getInstance();
  mainWindow->getOsgWidget()->renderBaseMesh(vertices, normals);
}

void ProjectStore::updateMetaData() {
  Lib3MF::PMetaDataGroup metaData = _project->GetMetaDataGroup();
  try {
    Lib3MF::PMetaData versionInformation = metaData->GetMetaDataByKey(META_NAMESPACE, "format");
  } catch (Lib3MF::ELib3MFException &e) {
    metaData->AddMetaData(META_NAMESPACE, "format", "1.0.0", "string", true);
  }

  json optiTrackData = json::array();
  for (OptiTrackPoint* optiTrackPoint: _optiTrackPoints) {
    optiTrackData.push_back({
      {"point", osgVecToStdVec(optiTrackPoint->getTranslation())},
      {"normal", osgVecToStdVec(optiTrackPoint->getNormal())},
      {"normalModifier", osgVecToStdVec(optiTrackPoint->getNormalModifier())},
      {"normalRotation", optiTrackPoint->getNormalRotation()},
      {"compensation", optiTrackPoint->getCompensation()},
      {"length", optiTrackPoint->getLength()},
      {"radius", optiTrackPoint->getRadius()}
    });
  }
  try {
    Lib3MF::PMetaData optiTrackPoints = metaData->GetMetaDataByKey(META_NAMESPACE, "optitrack");
    optiTrackPoints->SetValue(optiTrackData.dump());
  } catch (Lib3MF::ELib3MFException &e) {
    metaData->AddMetaData(META_NAMESPACE, "optitrack", optiTrackData.dump(), "string", true);
  }
  json emfTrackData = json::array();
  for (EMFTrackPoint* emfTrackPoint: _emfTrackPoints) {
    emfTrackData.push_back({
      {"point", osgVecToStdVec(emfTrackPoint->getTranslation())},
      {"normal", osgVecToStdVec(emfTrackPoint->getNormal())},
      {"normalModifier", osgVecToStdVec(emfTrackPoint->getNormalModifier())},
      {"normalRotation", emfTrackPoint->getNormalRotation()},
      {"compensation", emfTrackPoint->getCompensation()},
      {"width", emfTrackPoint->getWidth()},
      {"height", emfTrackPoint->getHeight()},
      {"depth", emfTrackPoint->getDepth()}
    });
  }
  try {
    Lib3MF::PMetaData emfTrackPoints = metaData->GetMetaDataByKey(META_NAMESPACE, "emftrack");
    emfTrackPoints->SetValue(emfTrackData.dump());
  } catch (Lib3MF::ELib3MFException &e) {
    metaData->AddMetaData(META_NAMESPACE, "emftrack", emfTrackData.dump(), "string", true);
  }
  json steamVrTrackData = json::array();
  for (SteamVRTrackPoint* steamVrTrackPoint: _steamVrTrackPoints) {
    steamVrTrackData.push_back({
      {"point", osgVecToStdVec(steamVrTrackPoint->getTranslation())},
      {"normal", osgVecToStdVec(steamVrTrackPoint->getNormal())},
      {"normalModifier", osgVecToStdVec(steamVrTrackPoint->getNormalModifier())},
      {"normalRotation", steamVrTrackPoint->getNormalRotation()},
      {"compensation", steamVrTrackPoint->getCompensation()},
      {"length", steamVrTrackPoint->getLength()}
    });
  }
  try {
    Lib3MF::PMetaData steamVrTrackPoints = metaData->GetMetaDataByKey(META_NAMESPACE, "steamvrtrack");
    steamVrTrackPoints->SetValue(steamVrTrackData.dump());
  } catch (Lib3MF::ELib3MFException &e) {
    metaData->AddMetaData(META_NAMESPACE, "steamvrtrack", steamVrTrackData.dump(), "string", true);
  }
  json actionPointData = json::array();
  for (ActionPoint* actionPoint: _actionPoints) {
    actionPointData.push_back({
      {"point", osgVecToStdVec(actionPoint->getTranslation())},
      {"normal", osgVecToStdVec(actionPoint->getNormal())},
      {"normalModifier", osgVecToStdVec(actionPoint->getNormalModifier())},
      {"normalRotation", actionPoint->getNormalRotation()},
      {"compensation", actionPoint->getCompensation()},
      {"identifier", actionPoint->getIdentifier()}
    });
  }
  try {
    Lib3MF::PMetaData actionPoints = metaData->GetMetaDataByKey(META_NAMESPACE, "actionpoints");
    actionPoints->SetValue(actionPointData.dump());
  } catch (Lib3MF::ELib3MFException &e) {
    metaData->AddMetaData(META_NAMESPACE, "actionpoints", actionPointData.dump(), "string", true);
  }
}

void ProjectStore::loadMetaData() {
  Lib3MF::PMetaDataGroup metaData = _project->GetMetaDataGroup();
  try {
    Lib3MF::PMetaData versionInformation = metaData->GetMetaDataByKey(META_NAMESPACE, "format");
  } catch (Lib3MF::ELib3MFException &e) {
    MainWindow::getInstance()->showErrorMessage("This is not a valid TrackpointApp project.", "Error opening project file.");
  }

  Lib3MF::PMetaData optiTrackString;
  try {
    optiTrackString = metaData->GetMetaDataByKey(META_NAMESPACE, "optitrack");
    auto optiTrackData = json::parse(optiTrackString->GetValue());
    _optiTrackPoints.clear();
    for (const auto pointData: optiTrackData) {
      osg::Vec3f point = osg::Vec3f(pointData["point"][0], pointData["point"][1], pointData["point"][2]);
      osg::Vec3f normal = osg::Vec3f(pointData["normal"][0], pointData["normal"][1], pointData["normal"][2]);
      osg::Vec3f normalModifier = osg::Vec3f(pointData["normalModifier"][0], pointData["normalModifier"][1], pointData["normalModifier"][2]);
      OptiTrackPoint* optiTrackPoint = new OptiTrackPoint(point, normal, normalModifier, static_cast<float>(pointData["normalRotation"]), static_cast<bool>(pointData["compensation"]), static_cast<double>(pointData["length"]), static_cast<double>(pointData["radius"]));
      _optiTrackPoints.push_back(optiTrackPoint);
    }
  } catch (Lib3MF::ELib3MFException &e) {
    MainWindow::getInstance()->showErrorMessage("Unable to load OptiTrack trackpoints.", "Error opening project file.");
  }
  Lib3MF::PMetaData emfTrackString;
  try {
    emfTrackString = metaData->GetMetaDataByKey(META_NAMESPACE, "emftrack");
    auto emfTrackData = json::parse(emfTrackString->GetValue());
    _emfTrackPoints.clear();
    for (const auto pointData: emfTrackData) {
      osg::Vec3f point = osg::Vec3f(pointData["point"][0], pointData["point"][1], pointData["point"][2]);
      osg::Vec3f normal = osg::Vec3f(pointData["normal"][0], pointData["normal"][1], pointData["normal"][2]);
      osg::Vec3f normalModifier = osg::Vec3f(pointData["normalModifier"][0], pointData["normalModifier"][1], pointData["normalModifier"][2]);
      EMFTrackPoint* emfTrackPoint = new EMFTrackPoint(point, normal, normalModifier, static_cast<float>(pointData["normalRotation"]), static_cast<bool>(pointData["compensation"]), static_cast<double>(pointData["width"]), static_cast<double>(pointData["height"]), static_cast<double>(pointData["depth"]));
      _emfTrackPoints.push_back(emfTrackPoint);
    }
  } catch (Lib3MF::ELib3MFException &e) {
    MainWindow::getInstance()->showErrorMessage("Unable to load EMFTrack trackpoints.", "Error opening project file.");
  }
  Lib3MF::PMetaData steamVrTrackString;
  try {
    steamVrTrackString = metaData->GetMetaDataByKey(META_NAMESPACE, "steamvrtrack");
    auto steamVrData = json::parse(steamVrTrackString->GetValue());
    _steamVrTrackPoints.clear();
    for (const auto pointData: steamVrData) {
      osg::Vec3f point = osg::Vec3f(pointData["point"][0], pointData["point"][1], pointData["point"][2]);
      osg::Vec3f normal = osg::Vec3f(pointData["normal"][0], pointData["normal"][1], pointData["normal"][2]);
      osg::Vec3f normalModifier = osg::Vec3f(pointData["normalModifier"][0], pointData["normalModifier"][1], pointData["normalModifier"][2]);
      SteamVRTrackPoint* steamVrTrackPoint = new SteamVRTrackPoint(point, normal, normalModifier, static_cast<float>(pointData["normalRotation"]), static_cast<bool>(pointData["compensation"]), static_cast<double>(pointData["length"]));
      _steamVrTrackPoints.push_back(steamVrTrackPoint);
    }
  } catch (Lib3MF::ELib3MFException &e) {
    MainWindow::getInstance()->showErrorMessage("Unable to load SteamVR trackpoints.", "Error opening project file.");
  }
  Lib3MF::PMetaData actionPointString;
  try {
    actionPointString = metaData->GetMetaDataByKey(META_NAMESPACE, "actionpoints");
    auto actionPointData = json::parse(actionPointString->GetValue());
    _actionPoints.clear();
    for (const auto pointData: actionPointData) {
      osg::Vec3f point = osg::Vec3f(pointData["point"][0], pointData["point"][1], pointData["point"][2]);
      osg::Vec3f normal = osg::Vec3f(pointData["normal"][0], pointData["normal"][1], pointData["normal"][2]);
      osg::Vec3f normalModifier = osg::Vec3f(pointData["normalModifier"][0], pointData["normalModifier"][1], pointData["normalModifier"][2]);
      ActionPoint* actionPoint = new ActionPoint(point, normal, normalModifier, static_cast<float>(pointData["normalRotation"]), static_cast<bool>(pointData["compensation"]), pointData["identifier"]);
      _actionPoints.push_back(actionPoint);
    }
  } catch (Lib3MF::ELib3MFException &e) {
    MainWindow::getInstance()->showErrorMessage("Unable to load ActionPoints.", "Error opening project file.");
  }
  render3MFMesh();
  MainWindow::getInstance()->renderView(Edit);
  MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(MainWindow::getInstance()->getEditWiget()->getSelectedTrackingSystem());
  MainWindow::getInstance()->getEditWiget()->updateTrackpointCount();
}

void ProjectStore::renderOptiTrackInThread(std::vector<OptiTrackPoint*> optiTrackPoints) {
  OpenScadRenderer* renderer = new OpenScadRenderer();
  renderer->renderOptiTrack(optiTrackPoints);
  delete renderer;
}

void ProjectStore::renderEMFTrackInThread(std::vector<EMFTrackPoint*> emfTrackPoints) {
  OpenScadRenderer* renderer = new OpenScadRenderer();
  renderer->renderEMFTrack(emfTrackPoints);
  delete renderer;
}

void ProjectStore::renderSteamVRTrackInThread(std::vector<SteamVRTrackPoint*> steamVrTrackPoints) {
  OpenScadRenderer* renderer = new OpenScadRenderer();
  renderer->renderSteamVRTrack(steamVrTrackPoints);
  delete renderer;
}

std::vector<float> ProjectStore::osgVecToStdVec(osg::Vec3f input) {
  std::vector<float> vector;
  vector.push_back(input.x());
  vector.push_back(input.y());
  vector.push_back(input.z());
  return vector;
}

osg::Vec3f ProjectStore::stdVecToOsgVec(std::vector<float> input) {
  return osg::Vec3f(input[0], input[1], input[2]);
}
