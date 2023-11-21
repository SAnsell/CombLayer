set (miraclesSources
    MIRACLES.cxx MIRACLESvariables.cxx 
)

add_library (miracles SHARED
  ${miraclesSources}
)
target_include_directories (miracles PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../miraclesInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/MIRACLES.cxx
  ${tarDIR}/MIRACLESvariables.cxx
  ${tarINC}/MIRACLES.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

