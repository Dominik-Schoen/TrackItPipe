#pragma once

// Include modules
#include "OptiTrackPoint.hpp"
#include "EMFTrackPoint.hpp"
#include "SteamVRTrackPoint.hpp"

// Include dependencies
#include <vector>

class OpenScadRenderer {
public:
  static bool openScadAvailable();
  void renderOptiTrack(std::vector<OptiTrackPoint*> points);
  void renderEMFTrack(std::vector<EMFTrackPoint*> points);
  void renderSteamVRTrack(std::vector<SteamVRTrackPoint*> points);

private:
  void enableSteamvrThread();
};
