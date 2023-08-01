set (filterSources
    FilterVariables.cxx makeFilter.cxx 
)

add_library (filter SHARED
  ${filterSources}
)
target_include_directories (filter PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/filterInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../filterInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/FilterVariables.cxx
  ${tarDIR}/makeFilter.cxx
  ${tarINC}/makeFilter.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

