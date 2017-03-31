set(PACKAGE RsnOut)

set(SRCS 
AliRsnOutValue.cxx
AliRsnOutTask.cxx
AliRsnOutTaskMgr.cxx
AliRsnOutTaskInput.cxx
AliRsnOutTaskBin.cxx
AliRsnOutTaskBinMgr.cxx
AliRsnOutTaskNorm.cxx
AliRsnOutTaskFit.cxx
AliRsnOutTaskRooFit.cxx
AliRsnOutTaskResult.cxx
)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}")
link_directories( "${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${ROOT_INCLUDE_DIR})
link_directories( ${ROOT_LIBRARY_DIR} )

string(REPLACE ".cxx" ".h" HDRS "${SRCS}")
if (ROOT_VERSION_MAJOR GREATER 5)
    ROOT_GENERATE_DICTIONARY( G__${PACKAGE} ${HDRS} MODULE ${PACKAGE} LINKDEF ${PACKAGE}LinkDef.h)
else (ROOT_VERSION_MAJOR GREATER 5)
    ROOT_GENERATE_DICTIONARY( G__${PACKAGE} ${HDRS} LINKDEF ${PACKAGE}LinkDef.h)
endif (ROOT_VERSION_MAJOR GREATER 5)
 list(APPEND SRCS G__${PACKAGE}.cxx)
 
add_library (${PACKAGE} SHARED ${SRCS} ${HDRS})
target_link_libraries (${PACKAGE} ${ROOT_LIBRARIES})

add_par(${PACKAGE})

install(TARGETS ${PACKAGE} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RUNTIME)
if (ROOT_VERSION_MAJOR GREATER 5)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${PACKAGE}_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/lib${PACKAGE}.rootmap DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif (ROOT_VERSION_MAJOR GREATER 5)

