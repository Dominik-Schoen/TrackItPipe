// Include own headers
#include "ProjectStore.hpp"

// Include modules
#include "MainWindow.hpp"
#include "StringBasics.hpp"
#include "TrackPointRenderer.hpp"

// Include dependencies
#include <typeinfo>
#include <iostream>

#define META_NAMESPACE "tk-ar-tracking"

ProjectStore::ProjectStore() {
  _projectLoaded = false;
  _projectModified = false;
  load3mfLib();
}

ProjectStore::~ProjectStore() {
}

void ProjectStore::loadMesh(std::string meshFile) {
  if (StringBasics::endsWithCaseInsensitive(meshFile, ".STL")) {
    printf("Currently unsupported.\n");
  } else if (StringBasics::endsWithCaseInsensitive(meshFile, ".3MF")) {
    _projectLoaded = true;
    // Read 3MF file
    Lib3MF::PReader reader = _project->QueryReader("3mf");
    reader->ReadFromFile(meshFile);
    render3MFMesh();
    MainWindow::getInstance()->renderView(Edit);
  } else {
    // TODO: Show error popup
    printf("Unsupported file type.\n");
  }
}

bool ProjectStore::loadProject(std::string projectFile) {
  if (!_projectLoaded) {
    Lib3MF::PReader reader = _project->QueryReader("3mf");
    reader->ReadFromFile(projectFile);
    _projectLoaded = true;
    _projectModified = false;
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
  updateMetaData();
  Lib3MF::PWriter writer = _project->QueryWriter("3mf");
  writer->WriteToFile(path);
  _projectFile = path;
  return true;
}

bool ProjectStore::exportProject(std::string path, ExportSettings settings) {
  return false; // TODO
}

void ProjectStore::updateNormalModifier(osg::Vec3 modifier) {
  _normalModifier = modifier;
}

osg::Vec3 ProjectStore::getNormalModifier() {
  return _normalModifier;
}

std::vector<OptiTrackPoint*> ProjectStore::getOptiTrackPoints() {
  return _optiTrackPoints;
}

void ProjectStore::addOptiTrackPoint(osg::Vec3 point, osg::Vec3 normal) {
  OptiTrackPoint* optiTrackPoint = new OptiTrackPoint(point, normal, _normalModifier, _optiTrackSettings.length, _optiTrackSettings.radius);
  _optiTrackPoints.push_back(optiTrackPoint);
}

void ProjectStore::removeOptiTrackPoint() {
  // TODO
}

void ProjectStore::updateOptiTrackSettings(OptiTrackSettings optiTrackSettings) {
  _optiTrackSettings = optiTrackSettings;
}

OptiTrackSettings ProjectStore::getOptiTrackSettings() {
  return _optiTrackSettings;
}

void ProjectStore::load3mfLib() {
  _wrapper = Lib3MF::CWrapper::loadLibrary();
  _project = _wrapper->CreateModel();
}

void ProjectStore::render3MFMesh() {
  // Get meshes
  Lib3MF::PMeshObjectIterator meshIterator = _project->GetMeshObjects();
  // Our use case supports just a single mesh per project
  if (meshIterator->Count() != 1) {
    // TODO: Show error popup
    printf("Not 1 mesh: %llu\n", meshIterator->Count());
    return;
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
  for (const Lib3MF::sTriangle triangle: triangleBuffer) {
    // Create osg vectors
    osg::Vec3 vertex[3] = {};
    for (char i = 0; i < 3; i++) {
      vertex[i] = osg::Vec3(verticesBuffer[triangle.m_Indices[i]].m_Coordinates[0], verticesBuffer[triangle.m_Indices[i]].m_Coordinates[1], verticesBuffer[triangle.m_Indices[i]].m_Coordinates[2]);
    }
    // Calculate normal
    osg::Vec3 edgeOne = vertex[1].operator-(vertex[0]);
    osg::Vec3 edgeTwo = vertex[2].operator-(vertex[0]);
    osg::Vec3 normal = edgeOne.operator^(edgeTwo);
    normal.normalize();
    // Store them
    for (int i = 0; i < 3; i++) {
      vertices->push_back(vertex[i]);
      normals->push_back(normal);
    }
  }
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
}

void ProjectStore::loadMetaData() {
  Lib3MF::PMetaDataGroup metaData = _project->GetMetaDataGroup();
  try {
    Lib3MF::PMetaData versionInformation = metaData->GetMetaDataByKey(META_NAMESPACE, "format");
  } catch (Lib3MF::ELib3MFException &e) {
    // TODO: Alert not a TrackpointApp poject
  }

  Lib3MF::PMetaData optiTrackString;
  try {
    optiTrackString = metaData->GetMetaDataByKey(META_NAMESPACE, "optitrack");
  } catch (Lib3MF::ELib3MFException &e) {
    // TODO: Something is wrong with the file
  }
  auto optiTrackData = json::parse(optiTrackString->GetValue());
  _optiTrackPoints.clear();
  for (const auto pointData: optiTrackData) {
    osg::Vec3f point = osg::Vec3f(pointData["point"][0], pointData["point"][1], pointData["point"][2]);
    osg::Vec3f normal = osg::Vec3f(pointData["normal"][0], pointData["normal"][1], pointData["normal"][2]);
    osg::Vec3f normalModifier = osg::Vec3f(pointData["normalModifier"][0], pointData["normalModifier"][1], pointData["normalModifier"][2]);
    OptiTrackPoint* optiTrackPoint = new OptiTrackPoint(point, normal, normalModifier, static_cast<double>(pointData["length"]), static_cast<double>(pointData["radius"]));
    _optiTrackPoints.push_back(optiTrackPoint);
  }
  render3MFMesh();
  MainWindow::getInstance()->renderView(Edit);
  MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(MainWindow::getInstance()->getEditWiget()->getSelectedTrackingSystem());
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
