set (cspecSources
    CSPEC.cxx CSPECvariables.cxx 
)

add_library (cspec SHARED
  ${cspecSources}
)
target_include_directories (cspec PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cspecInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/CSPEC.cxx
  ${tarDIR}/CSPECvariables.cxx
  ${tarINC}/CSPEC.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

