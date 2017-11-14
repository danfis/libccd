cmake_minimum_required(VERSION 2.8.11)

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

# Can not explicitly declared the software as C in project command due to bug:
# https://gitlab.kitware.com/cmake/cmake/issues/16967
project(libccd)

set(CCD_VERSION_MAJOR 2)
set(CCD_VERSION_MINOR 0)
set(CCD_VERSION ${CCD_VERSION_MAJOR}.${CCD_VERSION_MINOR})

set(CCD_SOVERSION 2)

# Include GNUInstallDirs to get canonical paths
include(GNUInstallDirs)
include(CTest)

option(BUILD_DOCUMENTATION "Build the documentation" OFF)

option(BUILD_SHARED_LIBS "Build libccd as a shared library" ON)

option(ENABLE_DOUBLE_PRECISION
  "Enable double precision computations instead of single precision" OFF)

# Option for some bundle-like build system in order not to expose
# any FCL binary symbols in their public ABI
option(CCD_HIDE_ALL_SYMBOLS "Hide all binary symbols" OFF)
if (CCD_HIDE_ALL_SYMBOLS)
  add_definitions("-DCCD_STATIC_DEFINE")
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

configure_package_config_file(ccd-config.cmake.in ccd-config.cmake
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/ccd"
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(ccd-config-version.cmake
  VERSION ${CCD_VERSION} COMPATIBILITY AnyNewerVersion)

install(FILES
  "${CMAKE_BINARY_DIR}/ccd-config.cmake"
  "${CMAKE_BINARY_DIR}/ccd-config-version.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/ccd")

set(CCD_PKGCONFIG_DESCRIPTION
  "Library for collision detection between convex shapes")
configure_file(ccd.pc.in ccd.pc @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/ccd.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

install(FILES BSD-LICENSE DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/doc/ccd")
