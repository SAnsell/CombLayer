set (polySources
    PolyFunction.cxx PolyVar.cxx PolyVarOne.cxx 
    solveValues.cxx 
)

add_library (poly SHARED
  ${polySources}
)
target_include_directories (poly PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../polyInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/PolyFunction.cxx
  ${tarDIR}/PolyVar.cxx
  ${tarDIR}/PolyVarOne.cxx
  ${tarDIR}/solveValues.cxx
  ${tarINC}/PolyFunction.h
  ${tarINC}/PolyVar.h
  ${tarINC}/solveValues.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

