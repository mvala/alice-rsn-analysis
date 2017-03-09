project(ROOTtut)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++1y")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#include_directories( "${CMAKE_SOURCE_DIR}/hlit")
#link_directories( "${CMAKE_BINARY_DIR}/hlit")

find_package(ROOT REQUIRED)
include(root-utils)

add_custom_target(par-all ALL)

add_subdirectory(RsnOut)

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
  "/build/;/.git/;/tmp/;~$;${CPACK_SOURCE_IGNORE_FILES}")
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source) 
include(CPack)

# uninstall
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
