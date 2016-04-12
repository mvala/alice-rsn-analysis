set(PACKAGE RsnOut)

set(SRCS 
AliRsnOutValue.cxx
AliRsnOutTask.cxx
AliRsnOutTaskMgr.cxx
AliRsnOutTaskInput.cxx
AliRsnOutTaskBin.cxx
AliRsnOutTaskNorm.cxx
AliRsnOutTaskFit.cxx
AliRsnOutTaskRooFit.cxx
AliRsnOutTaskResult.cxx
)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}")
link_directories( "${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${ROOT_INCLUDE_DIR})
link_directories( ${ROOT_LIBRARY_DIR} )

String(REPLACE ".cxx" ".h" HDRS "${SRCS}")
ROOT_GENERATE_DICTIONARY( G__${PACKAGE} ${HDRS} LINKDEF ${PACKAGE}LinkDef.h)
list(APPEND SRCS G__${PACKAGE}.cxx)

add_library (${PACKAGE} SHARED ${SRCS} ${HDRS})
target_link_libraries (${PACKAGE} ${ROOT_LIBRARIES})

add_par(${PACKAGE})
