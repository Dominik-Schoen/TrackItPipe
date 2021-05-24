// Include own headers
#include "ProjectStore.hpp"

// Include modules
#include "MainWindow.hpp"
#include "StringBasics.hpp"

#include "lib3mf_implicit.hpp"

#include <typeinfo>
#include <iostream>

ProjectStore::ProjectStore() {
  projectLoaded = false;
  //load3mfLib();
}

ProjectStore::ProjectStore(std::string projectFile) {

}

ProjectStore::~ProjectStore() {

}

void ProjectStore::load3mfLib() {
  //_wrapper = Lib3MF::CWrapper::loadLibrary();
}

void ProjectStore::loadMesh(std::string meshFile) {
  if (StringBasics::endsWithCaseInsensitive(meshFile, ".STL")) {
    printf("Currently unsupported.\n");
  } else if (StringBasics::endsWithCaseInsensitive(meshFile, ".3MF")) {
    Lib3MF::PWrapper _wrapper = Lib3MF::CWrapper::loadLibrary();
    Lib3MF::PModel _project = _wrapper->CreateModel();
    Lib3MF::PReader reader = _project->QueryReader("3mf");
    reader->ReadFromFile(meshFile);
    Lib3MF::PMeshObjectIterator meshIterator = _project->GetMeshObjects();
    if (meshIterator->Count() != 1) {
      printf("Not 1 mesh: %d\n", meshIterator->Count());
    }
    meshIterator->MoveNext();
    Lib3MF::PMeshObject mesh = meshIterator->GetCurrentMeshObject();
    std::vector<Lib3MF::sPosition> verticesBuffer;
    mesh->GetVertices(verticesBuffer);

    Lib3MF_uint64 vertex_count = mesh->GetVertexCount();
    Lib3MF_uint64 triangle_count = mesh->GetTriangleCount();
    for (Lib3MF_uint64 idx = 0; idx < triangle_count; ++idx) {
			Lib3MF::sTriangle triangle = mesh->GetTriangle(idx);
			Lib3MF::sPosition vertex1, vertex2, vertex3;

			vertex1 = mesh->GetVertex(triangle.m_Indices[0]);
			vertex2 = mesh->GetVertex(triangle.m_Indices[1]);
			vertex3 = mesh->GetVertex(triangle.m_Indices[2]);

			//p->append_poly();
			//p->append_vertex(vertex1.m_Coordinates[0], vertex1.m_Coordinates[1], vertex1.m_Coordinates[2]);
			//p->append_vertex(vertex2.m_Coordinates[0], vertex2.m_Coordinates[1], vertex2.m_Coordinates[2]);
			//p->append_vertex(vertex3.m_Coordinates[0], vertex3.m_Coordinates[1], vertex3.m_Coordinates[2]);
      printf("%f %f %f\n", vertex1.m_Coordinates[0], vertex1.m_Coordinates[1], vertex1.m_Coordinates[2]);
      printf("%f %f %f\n", vertex2.m_Coordinates[0], vertex2.m_Coordinates[1], vertex2.m_Coordinates[2]);
      printf("%f %f %f\n", vertex3.m_Coordinates[0], vertex3.m_Coordinates[1], vertex3.m_Coordinates[2]);
		}


    /*for (const Lib3MF::sPosition vertex: verticesBuffer) {
      std::cout << typeid(vertex).name() << std::endl;
      std::cout << typeid(vertex.m_Coordinates).name() << std::endl;
      std::cout << typeid(vertex.m_Coordinates[0]).name() << std::endl;
      printf("%f %f %f\n", (float)vertex.m_Coordinates[0], (float)vertex.m_Coordinates[1], (float)vertex.m_Coordinates[2]);
    }*/
    std::vector<Lib3MF::sTriangle> triangleBuffer;
    mesh->GetTriangleIndices(triangleBuffer);
    MainWindow* mainWindow = MainWindow::getInstance();
    mainWindow->getOsgWidget()->renderBaseMesh(verticesBuffer, triangleBuffer);
    printf("Done\n");
  } else {
    printf("Unsupported file type.\n"); // TODO: Show popup
  }
}
