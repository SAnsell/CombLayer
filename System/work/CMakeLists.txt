set (workSources
    BinData.cxx Boundary.cxx BUnit.cxx 
    WorkData.cxx 
)

add_library (work SHARED
  ${workSources}
)
target_include_directories (work PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../workInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BinData.cxx
  ${tarDIR}/Boundary.cxx
  ${tarDIR}/BUnit.cxx
  ${tarDIR}/WorkData.cxx
  ${tarINC}/BinData.h
  ${tarINC}/Boundary.h
  ${tarINC}/BUnit.h
  ${tarINC}/WorkData.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

