cmake_minimum_required(VERSION 2.8.11)

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

# Can not explicitly declared the software as C in project command due to bug:
# https://gitlab.kitware.com/cmake/cmake/issues/16967
project(libccddbl)

set(CCDDBL_VERSION_MAJOR 2)
set(CCDDBL_VERSION_MINOR 2)
set(CCDDBL_VERSION ${CCDDBL_VERSION_MAJOR}.${CCDDBL_VERSION_MINOR})

set(CCDDBL_SOVERSION 2)

# Include GNUInstallDirs to get canonical paths
include(GNUInstallDirs)
include(CTest)

option(BUILD_DOCUMENTATION "Build the documentation" OFF)

option(BUILD_SHARED_LIBS "Build libccddbl as a shared library" ON)

option(ENABLE_DOUBLE_PRECISION
  "Enable double precision computations instead of single precision" ON)

# Option for some bundle-like build system in order not to expose
# any FCL binary symbols in their public ABI
option(CCDDBL_HIDE_ALL_SYMBOLS "Hide all binary symbols" OFF)
if (CCDDBL_HIDE_ALL_SYMBOLS)
  add_definitions("-DCCDDBL_STATIC_DEFINE")
endif()

# set the default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build; options are Debug Release RelWithDebInfo MinSizeRel"
    FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
    STRINGS
      Debug
      Release
      RelWithDebInfo
      MinSizeRel)
endif()

add_subdirectory(src)

if(BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(ccddbl-config.cmake.in ccddbl-config.cmake
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/ccddbl"
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(ccddbl-config-version.cmake
  VERSION ${CCDDBL_VERSION} COMPATIBILITY AnyNewerVersion)

install(FILES
  "${CMAKE_BINARY_DIR}/ccddbl-config.cmake"
  "${CMAKE_BINARY_DIR}/ccddbl-config-version.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/ccddbl")

set(CCDDBL_PKGCONFIG_DESCRIPTION
  "Library for collision detection between convex shapes")
configure_file(ccddbl.pc.in ccddbl.pc @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/ccddbl.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

install(FILES BSD-LICENSE DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/doc/ccddbl")
