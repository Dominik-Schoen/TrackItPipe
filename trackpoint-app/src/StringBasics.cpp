// Include own headers
#include "StringBasics.hpp"

// Include dependencies
#include <algorithm>
#include <cctype>

// From https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
// Lowercase from https://stackoverflow.com/questions/313970/how-to-convert-an-instance-of-stdstring-to-lower-case
bool StringBasics::endsWithCaseInsensitive(std::string const &fullStringInput, std::string const &endingInput) {
  std::string fullString = fullStringInput;
  std::transform(fullString.begin(), fullString.end(), fullString.begin(), ::tolower);
  std::string ending = endingInput;
  std::transform(ending.begin(), ending.end(), ending.begin(), ::tolower);
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}
