#pragma once

#include "TrackPoint.hpp"

#include <vector>

class OpenScadRenderer {
public:
  void render(std::vector<TrackPoint*> points);
};
