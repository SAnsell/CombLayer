set (flukaPhysicsSources
    flukaPhysics.cxx 
)

add_library (flukaPhysics SHARED
  ${flukaPhysicsSources}
)
target_include_directories (flukaPhysics PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../flukaPhysicsInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/flukaPhysics.cxx
  ${tarINC}/flukaPhysics.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

