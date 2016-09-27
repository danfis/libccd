set(CU_SOURCES
  cu.c
  cu.h)

add_library(cu STATIC ${CU_SOURCES})

if(NOT APPLE)
  target_compile_definitions(cu PUBLIC CU_ENABLE_TIMER)
endif()

get_filename_component(CU_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
target_include_directories(cu PUBLIC "${CU_INCLUDE_DIR}")
