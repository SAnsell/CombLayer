set (singleItemBuildSources
    DefUnitsSingle.cxx makeSingleItem.cxx singleItemVariables.cxx 
)

add_library (singleItemBuild SHARED
  ${singleItemBuildSources}
)
target_include_directories (singleItemBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/singleItemBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../singleItemBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DefUnitsSingle.cxx
  ${tarDIR}/makeSingleItem.cxx
  ${tarDIR}/singleItemVariables.cxx
  ${tarINC}/DefUnitsSingleItem.h
  ${tarINC}/makeSingleItem.h
  ${tarINC}/singleItemVariables.h
  ${tarINC}/singleVariableSetup.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

