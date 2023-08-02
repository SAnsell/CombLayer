set (phitsPhysicsSources
    phitsPhysics.cxx 
)

add_library (phitsPhysics SHARED
  ${phitsPhysicsSources}
)
target_include_directories (phitsPhysics PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../phitsPhysicsInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/phitsPhysics.cxx
  ${tarINC}/phitsPhysics.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

