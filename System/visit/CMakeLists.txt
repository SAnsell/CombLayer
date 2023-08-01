set (visitSources
    FEM.cxx MatMD5.cxx MD5sum.cxx 
    Visit.cxx 
)

add_library (visit SHARED
  ${visitSources}
)
target_include_directories (visit PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../visitInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/FEM.cxx
  ${tarDIR}/MatMD5.cxx
  ${tarDIR}/MD5sum.cxx
  ${tarDIR}/Visit.cxx
  ${tarINC}/FEM.h
  ${tarINC}/MatMD5.h
  ${tarINC}/MD5sum.h
  ${tarINC}/Visit.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

