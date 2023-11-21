set (snsBuildSources
    makeSNS.cxx RefPlug.cxx SNSVariables.cxx 
    targetOuter.cxx 
)

add_library (snsBuild SHARED
  ${snsBuildSources}
)
target_include_directories (snsBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/snsBuildInc
   ${CMAKE_SOURCE_DIR}/Model/ralBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../snsBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/makeSNS.cxx
  ${tarDIR}/RefPlug.cxx
  ${tarDIR}/SNSVariables.cxx
  ${tarDIR}/targetOuter.cxx
  ${tarINC}/makeSNS.h
  ${tarINC}/RefPlug.h
  ${tarINC}/targetOuter.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

