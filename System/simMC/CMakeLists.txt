set (simMCSources
    SimMonte.cxx 
)

add_library (simMC SHARED
  ${simMCSources}
)
target_include_directories (simMC PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../simMCInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/SimMonte.cxx
  ${tarINC}/SimMonte.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

