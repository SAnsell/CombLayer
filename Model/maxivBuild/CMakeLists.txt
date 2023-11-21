set (maxivBuildSources
    DefUnitsMaxIV.cxx makeMaxIV.cxx maxivVariables.cxx 
)

add_library (maxivBuild SHARED
  ${maxivBuildSources}
)
target_include_directories (maxivBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/maxivBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../maxivBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DefUnitsMaxIV.cxx
  ${tarDIR}/makeMaxIV.cxx
  ${tarDIR}/maxivVariables.cxx
  ${tarINC}/DefUnitsMaxIV.h
  ${tarINC}/makeMaxIV.h
  ${tarINC}/maxivVariables.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

