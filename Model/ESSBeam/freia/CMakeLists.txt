set (freiaSources
    FREIA.cxx FREIAvariables.cxx 
)

add_library (freia SHARED
  ${freiaSources}
)
target_include_directories (freia PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../freiaInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/FREIA.cxx
  ${tarDIR}/FREIAvariables.cxx
  ${tarINC}/FREIA.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

