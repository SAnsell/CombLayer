set (pipeBuildSources
    DefUnitsPipe.cxx makePipe.cxx pipeTube.cxx 
    pipeVariables.cxx 
)

add_library (pipeBuild SHARED
  ${pipeBuildSources}
)
target_include_directories (pipeBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/pipeBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../pipeBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DefUnitsPipe.cxx
  ${tarDIR}/makePipe.cxx
  ${tarDIR}/pipeTube.cxx
  ${tarDIR}/pipeVariables.cxx
  ${tarINC}/DefUnitsPipe.h
  ${tarINC}/makePipe.h
  ${tarINC}/pipeTube.h
  ${tarINC}/pipeVariableSetup.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

