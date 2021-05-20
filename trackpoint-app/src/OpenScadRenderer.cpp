// Include own headers
#include "OpenScadRenderer.hpp"

// Include dependencies
#include <iostream>
#include <fstream>

const char* openScadBase =
  "$fn = 100;\n"
  "module optiTrackPointBase(translation, rotation) {\n"
  "translate(translation) rotate(rotation) cylinder(10, 1, 1, false);\n"
  "}\n";

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
  system("openscad -o /tmp/output.3mf /tmp/output.scad");
}
