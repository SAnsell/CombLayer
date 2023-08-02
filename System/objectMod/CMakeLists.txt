set (objectModSources
    mergeTemplate.cxx surfDivide.cxx 
)

add_library (objectMod SHARED
  ${objectModSources}
)
target_include_directories (objectMod PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../objectModInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/mergeTemplate.cxx
  ${tarDIR}/surfDivide.cxx
  ${tarINC}/mergeTemplate.h
  ${tarINC}/surfDivide.h
  ${tarINC}/surfExpand.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

