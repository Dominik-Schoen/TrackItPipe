#pragma once

// Include modules
#include "OptiTrackPoint.hpp"

// Include dependencies
#include <vector>

class OpenScadRenderer {
public:
  void renderOptiTrack(std::vector<OptiTrackPoint*> points);
};
