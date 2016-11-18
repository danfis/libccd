set(CU_SOURCES
  cu.c
  cu.h)

add_library(cu STATIC ${CU_SOURCES})

if(NOT APPLE)
  target_compile_definitions(cu PUBLIC CU_ENABLE_TIMER)
  find_library(LIBRT_LIBRARY NAMES rt)
  if(NOT LIBRT_LIBRARY)
    message(FATAL_ERROR "Could NOT find required library librt")
  endif()
  target_link_libraries(cu "${LIBRT_LIBRARY}")
endif()

get_filename_component(CU_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
target_include_directories(cu PUBLIC "${CU_INCLUDE_DIR}")
