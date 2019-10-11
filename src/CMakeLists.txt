if(DEFINED CCDDBL_SINGLE OR DEFINED CCDDBL_DOUBLE)
  # make sure only DOUBLE or SINGLE is set; default to SINGLE
  if(CCDDBL_SINGLE)
    set(CCDDBL_DOUBLE OFF)
  else()
    set(CCDDBL_SINGLE ON)
  endif()
  if(CCDDBL_DOUBLE)
    set(CCDDBL_SINGLE OFF)
  endif()
elseif(ENABLE_DOUBLE_PRECISION)
    set(CCDDBL_DOUBLE ON)
    set(CCDDBL_SINGLE OFF)
else()
    set(CCDDBL_DOUBLE OFF)
    set(CCDDBL_SINGLE ON)
endif()

configure_file(ccddbl/config.h.cmake.in ccddbl/config.h)

set(CCDDBL_INCLUDES
  ccddbl/ccddbl.h
  ccddbl/compiler.h
  ccddbl/ccddbl_export.h
  ccddbl/quat.h
  ccddbl/simplex.h
  ccddbl/support.h
  ccddbl/vec3.h
  "${CMAKE_CURRENT_BINARY_DIR}/ccddbl/config.h")

set(CCDDBL_SOURCES
  alloc.h
  ccddbl.c
  dbg.h
  list.h
  mpr.c
  polytope.c
  polytope.h
  support.c
  vec3.c)

add_library(ccddbl ${CCDDBL_INCLUDES} ${CCDDBL_SOURCES})
set_target_properties(ccddbl PROPERTIES
  PUBLIC_HEADER "${CCDDBL_INCLUDES}"
  SOVERSION ${CCDDBL_SOVERSION}
  VERSION ${CCDDBL_VERSION})
target_include_directories(ccddbl PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

if(NOT WIN32)
  find_library(LIBM_LIBRARY NAMES m)
  if(NOT LIBM_LIBRARY)
    message(FATAL_ERROR "Could NOT find required library LibM")
  endif()
  target_link_libraries(ccddbl "${LIBM_LIBRARY}")
  if(BUILD_SHARED_LIBS)
    set(CCDDBL_PKGCONFIG_EXTRA_LIBS -lm PARENT_SCOPE)
  endif()
endif()

export(TARGETS ccddbl FILE "${CMAKE_BINARY_DIR}/ccddbl-targets.cmake")

install(TARGETS ccddbl
  EXPORT ccddbl-targets
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/ccddbl"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(EXPORT ccddbl-targets DESTINATION "${CMAKE_INSTALL_LIBDIR}/ccddbl")

macro (check_compiler_visibility)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-fvisibility=hidden COMPILER_SUPPORTS_VISIBILITY)
endmacro()

if(UNIX)
  check_compiler_visibility()
  if (COMPILER_SUPPORTS_VISIBILITY)
    set_target_properties(ccddbl
         PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
  endif()
endif()

if(NOT WIN32 AND BUILD_TESTING AND NOT CCDDBL_HIDE_ALL_SYMBOLS)
  add_subdirectory(testsuites)
endif()
