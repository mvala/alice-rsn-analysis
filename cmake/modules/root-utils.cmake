function(add_par PACKAGE)
  if(PACKAGE)
  message(STATUS "Generating ${PACKAGE}")
  add_custom_target(${PACKAGE}.par
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}
    COMMAND cat ${CMAKE_SOURCE_DIR}/cmake/par/CMakeLists.txt.in > ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}/CMakeLists.txt
    COMMAND cat ${CMAKE_SOURCE_DIR}/${PACKAGE}/CMakeLists.txt | grep -v add_par>> ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}/CMakeLists.txt
    COMMAND cp -p ${CMAKE_SOURCE_DIR}/cmake/modules/FindROOT.cmake ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}/
    COMMAND cp -pR ${CMAKE_SOURCE_DIR}/${PACKAGE}/PROOF-INF ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}/
    COMMAND cp -pR ${CMAKE_SOURCE_DIR}/${PACKAGE}/*.h ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}/
    COMMAND cp -pR ${CMAKE_SOURCE_DIR}/${PACKAGE}/*.cxx ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}/
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}/pars tar --exclude=.svn -czhf ${CMAKE_SOURCE_DIR}/pars/${PACKAGE}.par ${PACKAGE}
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}/pars rm -rf ${PACKAGE}
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold "${PACKAGE}.par has been created in ${CMAKE_SOURCE_DIR}/pars"
#    DEPENDS ${PARSRCS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )

  add_dependencies(par-all ${PACKAGE}.par)

  endif(PACKAGE)
endfunction(add_par PACKAGE)
