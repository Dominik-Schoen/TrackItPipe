# Build Qt

SET(QT_PREFIX qt6)

SET(QT_URL https://download.qt.io/official_releases/qt/6.1/6.1.0/single/qt-everywhere-src-6.1.0.zip)

# Add Qt
FETCHCONTENT_DECLARE(
  ${QT_PREFIX}
  PREFIX ${QT_PREFIX}
  URL ${QT_URL}
)

FETCHCONTENT_GETPROPERTIES(${QT_PREFIX})

IF(NOT ${QT_PREFIX}_POPULATED)
  FETCHCONTENT_POPULATE(${QT_PREFIX})
  ADD_SUBDIRECTORY(${${QT_PREFIX}_SOURCE_DIR} ${${QT_PREFIX}_BINARY_DIR})
ENDIF()
