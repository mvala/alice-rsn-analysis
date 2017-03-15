# adding include path
include_directories(${ROOT_INCLUDE_DIR})

# adding lib dir path
link_directories( ${ROOT_LIBRARY_DIR} )

# linking test01 and ROOT libs
set(exe_file rsn-out-run)
add_executable (${exe_file} ${exe_file}.cxx)
target_link_libraries (${exe_file} ${ROOT_LIBRARIES} RsnOut RooFit)
