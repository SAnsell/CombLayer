set (bifrostSources
    BIFROST.cxx BIFROSTvariables.cxx 
)

add_library (bifrost SHARED
  ${bifrostSources}
)
target_include_directories (bifrost PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bifrostInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BIFROST.cxx
  ${tarDIR}/BIFROSTvariables.cxx
  ${tarINC}/BIFROST.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

