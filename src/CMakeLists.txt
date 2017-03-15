# adding include path
include_directories(${CMAKE_SOURCE_DIR}/RsnOut ${ROOT_INCLUDE_DIR} ${JSONCPP_INCLUDE_DIRS})

# adding lib dir path
link_directories( ${ROOT_LIBRARY_DIR})

# linking test01 and ROOT libs
set(exe_file rsn-out-run)
add_executable (${exe_file} ${exe_file}.cxx)
target_link_libraries (${exe_file} ${ROOT_LIBRARIES} ${JSONCPP_LIBRARIES} RsnOut RooFit)
