set (simpleItemSources
    simpleITEM.cxx simpleITEMvariables.cxx 
)

add_library (simpleItem SHARED
  ${simpleItemSources}
)
target_include_directories (simpleItem PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../simpleItemInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/simpleITEM.cxx
  ${tarDIR}/simpleITEMvariables.cxx
  ${tarINC}/simpleITEM.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

