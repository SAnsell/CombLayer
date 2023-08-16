set (trexSources
    TREX.cxx TrexHut.cxx TREXvariables.cxx 
)

add_library (trex SHARED
  ${trexSources}
)
target_include_directories (trex PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../trexInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/TREX.cxx
  ${tarDIR}/TrexHut.cxx
  ${tarDIR}/TREXvariables.cxx
  ${tarINC}/TREX.h
  ${tarINC}/TrexHut.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

