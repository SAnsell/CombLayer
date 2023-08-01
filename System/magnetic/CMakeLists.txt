set (magneticSources
    elecUnit.cxx magnetUnit.cxx SetMagnets.cxx 
)

add_library (magnetic SHARED
  ${magneticSources}
)
target_include_directories (magnetic PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../magneticInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/elecUnit.cxx
  ${tarDIR}/magnetUnit.cxx
  ${tarDIR}/SetMagnets.cxx
  ${tarINC}/elecUnit.h
  ${tarINC}/magnetUnit.h
  ${tarINC}/SetMagnets.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

