set (d4cModelSources
    d4cVariables.cxx DetectorArray.cxx DetectorBank.cxx 
    makeD4C.cxx 
)

add_library (d4cModel SHARED
  ${d4cModelSources}
)
target_include_directories (d4cModel PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/d4cModelInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../d4cModelInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/d4cVariables.cxx
  ${tarDIR}/DetectorArray.cxx
  ${tarDIR}/DetectorBank.cxx
  ${tarDIR}/makeD4C.cxx
  ${tarINC}/DetectorArray.h
  ${tarINC}/DetectorBank.h
  ${tarINC}/makeD4C.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

