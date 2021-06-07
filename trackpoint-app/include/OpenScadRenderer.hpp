#pragma once

// Include modules
#include "OptiTrackPoint.hpp"

// Include dependencies
#include <vector>

class OpenScadRenderer {
public:
  static bool openScadAvailable();
  void renderOptiTrack(std::vector<OptiTrackPoint*> points);
};
