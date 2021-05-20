// Include own headers
#include "ProjectStore.hpp"

// Include modules
#include "StringBasics.hpp"

ProjectStore::ProjectStore() {
  projectLoaded = false;
  load3mfLib();
}

ProjectStore::ProjectStore(QString projectFile) {

}

void ProjectStore::load3mfLib() {
  _wrapper = Lib3MF::CWrapper::loadLibrary();
}

void ProjectStore::loadMesh(QString meshFileString) {
  std::string meshFile = meshFileString.toUtf8().constData();
  if (StringBasics::endsWithCaseInsensitive(meshFile, ".STL")) {
    printf("Currently unsupported.\n");
  } else if (StringBasics::endsWithCaseInsensitive(meshFile, ".3MF")) {
    Lib3MF::PModel meshModel = _wrapper->CreateModel();
    Lib3MF::PReader reader = meshModel->QueryReader("3mf");
    reader->ReadFromFile(meshFile);
  } else {
    printf("Unsupported file type.\n"); // TODO: Show popup
  }
}
