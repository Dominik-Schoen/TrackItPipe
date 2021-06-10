// Include own headers
#include "ActionPoint.hpp"

ActionPoint::ActionPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const std::string identifier): TrackPoint(point, normal, normalModifier) {
  _identifier = identifier;
}

std::string ActionPoint::getIdentifier() {
  return _identifier;
}

ActionPointSettings ActionPoint::getActionPointSettings() {
  return ActionPointSettings {_identifier};
}

void ActionPoint::updateActionPointSettings(ActionPointSettings settings) {
  _identifier = settings.identifier;
}
