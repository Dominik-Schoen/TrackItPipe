// Include own headers
#include "OpenScadRenderer.hpp"

// Include dependencies
#include <iostream>
#include <fstream>
#include <sstream>

const char* openScadBase =
  "$fn = 100;\n"
  "module optiTrackPointBase(translation, rotation) {\n"
  "translate(translation) rotate(rotation) cylinder(10, 1, 1, false);\n"
  "}\n";

#if __APPLE__
  std::string openScadPath = "/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD";
#elif __unix__
  std::string openScadPath = "openscad";
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
  // Currently unsupported
#endif

void OpenScadRenderer::render(std::vector<TrackPoint*> points) {
  std::ofstream scadFile;
  scadFile.open("/tmp/output.scad");
  scadFile << openScadBase;
  scadFile << "import(\"testbutton.stl\");\n";
  for (TrackPoint* point: points) {
    osg::Vec3 translation = point->getTranslation();
    osg::Vec3 rotation = point->getRotation();
    scadFile << "optiTrackPointBase([" << translation.x() << "," << translation.y() << "," << translation.z() << "], [" << rotation.x() << "," << rotation.y() << "," << rotation.z() << "]);\n";
  }
  scadFile.close();
  //std::string command = openScadPath + " -o " + std::filesystem::temp_directory_path() + "/output.3mf " + std::filesystem::temp_directory_path() + "/output.scad";
  //system(command);
}
