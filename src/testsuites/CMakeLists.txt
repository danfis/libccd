add_subdirectory(cu)

set(MAIN_SOURCES
  main.c
  common.c
  common.h
  support.c
  support.h
  vec3.c
  vec3.h
  polytope.c
  polytope.h
  boxbox.c
  boxbox.h
  spheresphere.c
  spheresphere.h
  cylcyl.c
  cylcyl.h
  boxcyl.c
  boxcyl.h
  mpr_boxbox.c
  mpr_boxbox.h
  mpr_cylcyl.c
  mpr_cylcyl.h
  mpr_boxcyl.c
  mpr_boxcyl.h
  support.c
  support.h)

add_executable(main ${MAIN_SOURCES})
target_link_libraries(main ccd cu)
add_test(NAME main COMMAND main)


if(NOT APPLE)
  set(BENCH_SOURCES
    bench.c
    support.c
    support.h)

  add_executable(bench ${BENCH_SOURCES})
  target_link_libraries(bench ccd cu)
  add_test(NAME bench COMMAND bench)

  set(BENCH2_SOURCES
    bench2.c
    support.c
    support.h)

  add_executable(bench2 ${BENCH2_SOURCES})
  target_link_libraries(bench2 ccd cu)
  add_test(NAME bench2 COMMAND bench2)
endif()
