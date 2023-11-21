set (nmxSources
    NMX.cxx NMXvariables.cxx 
)

add_library (nmx SHARED
  ${nmxSources}
)
target_include_directories (nmx PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nmxInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/NMX.cxx
  ${tarDIR}/NMXvariables.cxx
  ${tarINC}/NMX.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

