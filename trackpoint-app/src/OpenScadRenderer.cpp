// Include own headers
#include "OpenScadRenderer.hpp"

// Include modules
#include "PlatformSupport.hpp"

// Include dependencies
#include <iostream>
#include <fstream>
#include <filesystem>

const char* openScadBase =
  "$fn = 100;\n"
  "module optiTrackPointBase(translation, rotation, length, radius) {\n"
  "translate(translation) rotate(rotation) cylinder(length, radius, radius, false);\n"
  "}\n";

void OpenScadRenderer::renderOptiTrack(std::vector<OptiTrackPoint*> points) {
  std::ofstream scadFile;
  scadFile.open(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_optitrack.scad");
  scadFile << openScadBase;
  scadFile << "import(\"trackpointapp_export.3mf\");\n";
  for (OptiTrackPoint* point: points) {
    osg::Vec3 translation = point->getTranslation();
    osg::Vec3 rotation = point->getRotation();
    scadFile << "optiTrackPointBase([" << translation.x() << "," << translation.y() << "," << translation.z() << "], [" << rotation.x() << "," << rotation.y() << "," << rotation.z() << "], " << point->getLength() << ", " << point->getRadius() << ");\n";
  }
  scadFile.close();
  std::string command = openScadPath + " -o " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_render_optitrack.3mf " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_optitrack.scad";
  system(command.c_str());
}
