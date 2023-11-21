set (magicSources
    MAGIC.cxx MAGICvariables.cxx 
)

add_library (magic SHARED
  ${magicSources}
)
target_include_directories (magic PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../magicInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/MAGIC.cxx
  ${tarDIR}/MAGICvariables.cxx
  ${tarINC}/MAGIC.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

