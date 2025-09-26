set (multiSources
  multiData.cxx  multiIndex.cxx
)

add_library (multi SHARED
  ${multiSources}
)
target_include_directories (multi PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../multiInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/multiData.cxx
  ${tarDIR}/multiIndex.cxx
  ${tarINC}/dataSlice.h
  ${tarINC}/multiData.h
  ${tarINC}/multiIndex.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

