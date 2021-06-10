#pragma once

// Include modules
#include "defaults.hpp"
#include "TrackPoint.hpp"
#include "TrackSystemSettingsStructs.hpp"

class ActionPoint: public TrackPoint {
public:
  ActionPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const std::string identifier = ACTIONPOINT_DEFAULT_IDENFIFIER);
  std::string getIdentifier();
  ActionPointSettings getActionPointSettings();
  void updateActionPointSettings(ActionPointSettings settings);

private:
  std::string _identifier;
};
