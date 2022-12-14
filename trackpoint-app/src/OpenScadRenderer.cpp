// Include own headers
#include "OpenScadRenderer.hpp"

// Include modules
#include "PlatformSupport.hpp"
#include "scad.hpp"

// Include dependencies
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

const char* openScadBase =
  "$fn = 100;\n"
  "use <threads.scad>\n"
  "module optiTrackPointBase(translation, rotation, length, radius, compensation) {\n"
  "translate(translation) rotate(rotation) {\n"
  "translate([0, 0, -compensation]) cylinder(length + compensation - 5, radius, radius, false);\n"
  "translate([0, 0, length - 5]) cylinder(5, 0.74, 0.74, false);\n"
  "}\n"
  "}\n"
  "module emfTrackPointBase(translation, rotation, width, height, depth, compensation) {\n"
  "translate(translation) rotate(rotation) {\n"
  "translate([-(width / 2), -(height / 2), -compensation]) cube([width, height, depth + compensation]);"
  "}\n"
  "}\n"
  "module steamVrTrackPointBase(translation, rotation, length, compensation) {\n"
  "translate(translation) rotate(rotation) {\n"
  "translate([0, 0, -compensation]) cylinder(length + compensation, 3.5, 3.5, false);\n"
  "translate([0, 0, length]) english_thread(diameter=1/4, threads_per_inch=20, length=0.393701);\n"
  "}\n"
  "}\n";

bool OpenScadRenderer::openScadAvailable() {
  std::string path = openScadPath + " -v";
  int result = system(path.c_str());
  return result == 0;
}

void OpenScadRenderer::renderOptiTrack(std::vector<OptiTrackPoint*> points) {
  std::ofstream scadFile;
  scadFile.open(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_optitrack.scad");
  scadFile << openScadBase;
  scadFile << "import(\"trackpointapp_export.3mf\");\n";
  for (OptiTrackPoint* point: points) {
    osg::Vec3 translation = point->getTranslation();
    osg::Vec3 rotation = point->getRotation();
    scadFile << "optiTrackPointBase([" << translation.x() << "," << translation.y() << "," << translation.z() << "], [" << rotation.x() << "," << rotation.y() << "," << rotation.z() << "], " << point->getLength() << ", " << point->getRadius() << ", " << point->getNormalCompensation() << ");\n";
  }
  scadFile.close();
  std::string command = openScadPath + " -o " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_render_optitrack.3mf " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_optitrack.scad";
  system(command.c_str());
}

void OpenScadRenderer::renderEMFTrack(std::vector<EMFTrackPoint*> points) {
  std::ofstream scadFile;
  scadFile.open(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_emftrack.scad");
  scadFile << openScadBase;
  scadFile << "difference() {\nimport(\"trackpointapp_export.3mf\");\n";
  for (EMFTrackPoint* point: points) {
    osg::Vec3 translation = point->getTranslation();
    osg::Vec3 rotation = point->getRotation();
    scadFile << "emfTrackPointBase([" << translation.x() << "," << translation.y() << "," << translation.z() << "], [" << rotation.x() << "," << rotation.y() << "," << rotation.z() << "], " << point->getWidth() << ", " << point->getHeight() << ", " << point->getDepth() << ", " << point->getNormalCompensation() << ");\n";
  }
  scadFile << "}\n";
  scadFile.close();
  std::string command = openScadPath + " -o " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_render_emftrack.3mf " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_emftrack.scad";
  system(command.c_str());
}

void OpenScadRenderer::renderSteamVRTrack(std::vector<SteamVRTrackPoint*> points) {
  enableSteamvrThread();
  std::ofstream scadFile;
  scadFile.open(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_steamvrtrack.scad");
  scadFile << openScadBase;
  scadFile << "import(\"trackpointapp_export.3mf\");\n";
  for (SteamVRTrackPoint* point: points) {
    osg::Vec3 translation = point->getTranslation();
    osg::Vec3 rotation = point->getRotation();
    scadFile << "steamVrTrackPointBase([" << translation.x() << "," << translation.y() << "," << translation.z() << "], [" << rotation.x() << "," << rotation.y() << "," << rotation.z() << "], " << point->getLength() << ", " << point->getNormalCompensation() << ");\n";
  }
  scadFile.close();
  std::string command = openScadPath + " -o " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_render_steamvrtrack.3mf " + std::filesystem::temp_directory_path().u8string() + fileDelimiter + "trackpointapp_export_steamvrtrack.scad";
  system(command.c_str());
}

void OpenScadRenderer::enableSteamvrThread() {
  std::ofstream resourceFile;
  resourceFile.open(std::filesystem::temp_directory_path().u8string() + fileDelimiter + "threads.scad");
  resourceFile << threads_SCAD;
  resourceFile.close();
}
