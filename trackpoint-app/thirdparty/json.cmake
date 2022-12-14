# Build Json

# The GitHub release (tag) we want to use
SET(JSON_TAG v3.10.2)

SET(JSON_PREFIX nlohmann_json)

SET(JSON_URL https://github.com/nlohmann/json)

# Configure Json
SET(JSON_BuildTests OFF CACHE INTERNAL "")

# Add Json
FETCHCONTENT_DECLARE(
  ${JSON_PREFIX}
  PREFIX ${JSON_PREFIX}
  GIT_REPOSITORY ${JSON_URL}
  GIT_TAG ${JSON_TAG}
)

FETCHCONTENT_GETPROPERTIES(${JSON_PREFIX})

IF(NOT ${JSON_PREFIX}_POPULATED)
  FETCHCONTENT_POPULATE(${JSON_PREFIX})
  ADD_SUBDIRECTORY(${${JSON_PREFIX}_SOURCE_DIR} ${${JSON_PREFIX}_BINARY_DIR})
ENDIF()
