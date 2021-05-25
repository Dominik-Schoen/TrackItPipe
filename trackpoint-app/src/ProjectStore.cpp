// Include own headers
#include "ProjectStore.hpp"

// Include modules
#include "MainWindow.hpp"
#include "StringBasics.hpp"

#include <typeinfo>
#include <iostream>

ProjectStore::ProjectStore() {
  projectLoaded = false;
  load3mfLib();
}

ProjectStore::ProjectStore(std::string projectFile) {

}

ProjectStore::~ProjectStore() {

}

void ProjectStore::loadMesh(std::string meshFile) {
  if (StringBasics::endsWithCaseInsensitive(meshFile, ".STL")) {
    printf("Currently unsupported.\n");
  } else if (StringBasics::endsWithCaseInsensitive(meshFile, ".3MF")) {
    // Create new project model (because we don't have one if we load a mesh)
    _project = _wrapper->CreateModel();
    // Read 3MF file
    Lib3MF::PReader reader = _project->QueryReader("3mf");
    reader->ReadFromFile(meshFile);
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
    render3MFMesh(verticesBuffer, triangleBuffer);
  } else {
    // TODO: Show error popup
    printf("Unsupported file type.\n");
  }
}

void ProjectStore::load3mfLib() {
  _wrapper = Lib3MF::CWrapper::loadLibrary();
}

void ProjectStore::render3MFMesh(const std::vector<Lib3MF::sPosition> verticesBuffer, const std::vector<Lib3MF::sTriangle> triangleBuffer) {
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
      printf("%f %f %f\n", vertex[i].x(), vertex[i].y(), vertex[i].z());
      vertices->push_back(vertex[i]);
      normals->push_back(normal);
    }
  }
  // Call renderer
  MainWindow* mainWindow = MainWindow::getInstance();
  mainWindow->getOsgWidget()->renderBaseMesh(vertices, normals);
}
