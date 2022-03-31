project(alice-rsn-analysis)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++1y")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_INSTALL_LIBDIR lib)

find_package(ROOT REQUIRED)
string(REPLACE "." ";" VERSION_LIST ${ROOT_VERSION})
list(GET VERSION_LIST 0 ROOT_VERSION_MAJOR)
if (ROOT_VERSION_MAJOR GREATER 5)
  include(Root6Macros)
else (ROOT_VERSION_MAJOR GREATER 5)
  add_compile_options("-std=c++1y")
endif (ROOT_VERSION_MAJOR GREATER 5)
include(RootMacros)

find_package(JSONCPP REQUIRED)

add_custom_target(par-all ALL)

add_subdirectory(RsnOut)
add_subdirectory(src)

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
