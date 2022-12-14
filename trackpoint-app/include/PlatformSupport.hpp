#pragma once

#if __APPLE__
  static std::string openScadPath = "/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD";
  static std::string fileDelimiter = "/";
#elif __unix__
  static std::string openScadPath = "openscad";
  static std::string fileDelimiter = "/";
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
  static std::string openScadPath = "\"C:\\Program Files\\OpenSCAD\\openscad.exe\"";
  static std::string fileDelimiter = "\\";
#endif
