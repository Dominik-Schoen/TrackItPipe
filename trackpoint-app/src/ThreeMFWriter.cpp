// Include own headers
#include "ThreeMFWriter.hpp"

ThreeMFWriter::ThreeMFWriter() {
  _wrapper = Lib3MF::CWrapper::loadLibrary();
}

void ThreeMFWriter::writeTrackPoints(std::vector<TrackPoint*> points, std::string path) {
  // Load the file created by OpenSCAD
  Lib3MF::PModel model = _wrapper->CreateModel();
  Lib3MF::PReader reader = model->QueryReader("3mf");
  reader->ReadFromFile("/tmp/output.3mf");

  Lib3MF::PMetaDataGroup metaData = model->GetMetaDataGroup();

  json trackpointData;
  std::vector<std::vector<float>> pointsList;
  for (TrackPoint* point: points) {
    std::vector<float> pointData;
    osg::Vec3 trackPoint = point->getTrackPoint();
    pointData.push_back(trackPoint.x());
    pointData.push_back(trackPoint.y());
    pointData.push_back(trackPoint.z());
    pointsList.push_back(pointData);
  }
  trackpointData["trackpoints"] = {
    {"tracking-system", "optitrack"},
    {"trackpoints", pointsList}
  };
  metaData->AddMetaData("tk-ar-tracking", "trackpoints", trackpointData.dump(), "string", true);

  Lib3MF::PWriter writer = model->QueryWriter("3mf");
  writer->WriteToFile(path);
}
