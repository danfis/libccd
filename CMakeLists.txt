cmake_minimum_required(VERSION 2.8)
project(libccd C)

include_directories("src/")
include_directories("src/testsuites/")

# make sure only DOUBLE or SINGLE is set; default to SINGLE
if (${CCD_SINGLE})
  set(CCD_DOUBLE false)
else()
  set(CCD_SINGLE true)
endif()

if (${CCD_DOUBLE})
  set(CCD_SINGLE false)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/ccd/config.h.cmake.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/ccd/config.h")
add_definitions(-DCU_ENABLE_TIMER)

find_library(MATH m)
find_library(RT rt)

add_library(ccd STATIC 
  src/alloc.c
  src/ccd.c
  src/mpr.c
  src/polytope.c
  src/support.c
  src/vec3.c
)
target_link_libraries(ccd ${MATH})

install(TARGETS ccd DESTINATION lib/)
install(DIRECTORY src/ccd DESTINATION include FILES_MATCHING PATTERN "*.h")
