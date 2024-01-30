set (exampleBuildSources
  exampleVariables.cxx
  makeExample.cxx 
)

add_library (exampleBuild SHARED
  ${exampleBuildSources}
)
target_include_directories (exampleBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/exampleBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../exampleBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/exampleVariables.cxx
  ${tarDIR}/makeExample.cxx
  ${tarINC}/makeExample.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

