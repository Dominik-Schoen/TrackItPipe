#pragma once

#include <string>

typedef struct {
  bool OptiTrack;
  bool EMFTrack;
  bool SteamVRTrack;
} ExportSettings;

typedef struct {
  double length;
  double radius;
} OptiTrackSettings;

typedef struct {
  double width;
  double height;
  double depth;
} EMFTrackSettings;

typedef struct {
  double length;
} SteamVRTrackSettings;

typedef struct {
  std::string identifier;
} ActionPointSettings;
