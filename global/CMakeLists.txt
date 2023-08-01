set (globalSources
    XMLwriteVisitor.cxx 
)

add_library (global SHARED
  ${globalSources}
)
target_include_directories (global PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../globalInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/XMLwriteVisitor.cxx
  ${tarINC}/BaseModVisit.h
  ${tarINC}/BaseVisit.h
  ${tarINC}/XMLwriteVisitor.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

