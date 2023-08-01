set (worldSources
    World.cxx 
)

add_library (world SHARED
  ${worldSources}
)
target_include_directories (world PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../worldInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/World.cxx
  ${tarINC}/World.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

