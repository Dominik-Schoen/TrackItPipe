#pragma once

#include "TrackPoint.hpp"

#include "lib3mf_implicit.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ThreeMFWriter {
public:
  ThreeMFWriter();
  void writeTrackPoints(std::vector<TrackPoint*> points, std::string path);

private:
  Lib3MF::PWrapper _wrapper;
};
