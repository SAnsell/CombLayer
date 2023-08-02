set (compWeightsSources
    Mesh.cxx 
)

add_library (compWeights SHARED
  ${compWeightsSources}
)
target_include_directories (compWeights PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../compWeightsInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Mesh.cxx
  ${tarINC}/Mesh.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

