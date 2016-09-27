find_program(SPHINX_EXECUTABLE NAMES sphinx-build sphinx-build2)

if(NOT SPHINX_EXECUTABLE)
  message(FATAL_ERROR "Could NOT find required executable sphinx-build")
endif()

add_custom_target(doc ALL)

set(CCD_DOCTREE_DIR "${CMAKE_CURRENT_BINARY_DIR}/.doctrees")
set(CCD_HTML_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

add_custom_target(html COMMAND
  "${SPHINX_EXECUTABLE}" -b html -d "${CCD_DOCTREE_DIR}" -q
  "${CMAKE_CURRENT_SOURCE_DIR}" "${CCD_HTML_OUTPUT_DIR}")
add_dependencies(doc html)

install(DIRECTORY "${CCD_HTML_OUTPUT_DIR}"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/doc/ccd")

set(CCD_DOC_ADDITIONAL_MAKE_CLEAN_FILES
  "${CCD_DOCTREE_DIR}"
  "${CCD_HTML_OUTPUT_DIR}")

if(NOT WIN32)
  set(CCD_MAN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/man")

  add_custom_target(man COMMAND
    "${SPHINX_EXECUTABLE}" -b man -d "${CCD_DOCTREE_DIR}" -q
    "${CMAKE_CURRENT_SOURCE_DIR}" "${CCD_MAN_OUTPUT_DIR}")
  add_dependencies(doc man)

  install(DIRECTORY "${CCD_MAN_OUTPUT_DIR}/"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")

  list(APPEND CCD_DOC_ADDITIONAL_MAKE_CLEAN_FILES "${CCD_MAN_OUTPUT_DIR}")
endif()

set_directory_properties(PROPERTIES
  ADDITIONAL_MAKE_CLEAN_FILES ${CCD_DOC_ADDITIONAL_MAKE_CLEAN_FILES})
