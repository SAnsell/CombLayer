set (skadiSources
    SKADI.cxx SkadiHut.cxx SKADIvariables.cxx 
)

add_library (skadi SHARED
  ${skadiSources}
)
target_include_directories (skadi PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../skadiInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/SKADI.cxx
  ${tarDIR}/SkadiHut.cxx
  ${tarDIR}/SKADIvariables.cxx
  ${tarINC}/SKADI.h
  ${tarINC}/SkadiHut.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

