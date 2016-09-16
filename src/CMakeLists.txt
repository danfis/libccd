if(DEFINED CCD_SINGLE OR DEFINED CCD_DOUBLE)
  # make sure only DOUBLE or SINGLE is set; default to SINGLE
  if(CCD_SINGLE)
    set(CCD_DOUBLE OFF)
  else()
    set(CCD_SINGLE ON)
  endif()
  if(CCD_DOUBLE)
    set(CCD_SINGLE OFF)
  endif()
elseif(ENABLE_DOUBLE_PRECISION)
    set(CCD_DOUBLE ON)
    set(CCD_SINGLE OFF)
else()
    set(CCD_DOUBLE OFF)
    set(CCD_SINGLE ON)
endif()

configure_file(ccd/config.h.cmake.in ccd/config.h)

set(CCD_INCLUDES
  ccd/ccd.h
  ccd/compiler.h
  ccd/quat.h
  ccd/vec3.h
  "${CMAKE_CURRENT_BINARY_DIR}/ccd/config.h")

set(CCD_SOURCES
  alloc.h
  ccd.c
  dbg.h
  list.h
  mpr.c
  polytope.c
  polytope.h
  simplex.h
  support.c
  support.h
  vec3.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(ccd ${CCD_INCLUDES} ${CCD_SOURCES})
set_target_properties(ccd PROPERTIES
  PUBLIC_HEADER "${CCD_INCLUDES}"
  SOVERSION ${CCD_SOVERSION}
  VERSION ${CCD_VERSION})

if(NOT WIN32)
  find_library(LIBM_LIBRARY NAMES m)
  if(NOT LIBM_LIBRARY)
    message(FATAL_ERROR "Could NOT find required library LibM")
  endif()
  target_link_libraries(ccd "${LIBM_LIBRARY}")
  if(BUILD_SHARED_LIBS)
    set(CCD_PKGCONFIG_EXTRA_LIBS -lm PARENT_SCOPE)
  endif()
endif()

install(TARGETS ccd
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/ccd"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
