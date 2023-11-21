set (maxpeemSources
    MAXPEEM.cxx maxpeemFrontEnd.cxx maxpeemOpticsHut.cxx 
    maxpeemOpticsLine.cxx maxpeemVariables.cxx 
)

add_library (maxpeem SHARED
  ${maxpeemSources}
)
target_include_directories (maxpeem PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../maxpeemInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/MAXPEEM.cxx
  ${tarDIR}/maxpeemFrontEnd.cxx
  ${tarDIR}/maxpeemOpticsHut.cxx
  ${tarDIR}/maxpeemOpticsLine.cxx
  ${tarDIR}/maxpeemVariables.cxx
  ${tarINC}/maxpeemFrontEnd.h
  ${tarINC}/MAXPEEM.h
  ${tarINC}/maxpeemOpticsHut.h
  ${tarINC}/maxpeemOpticsLine.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

