set (dreamSources
    DREAM.cxx DREAMvariables.cxx 
)

add_library (dream SHARED
  ${dreamSources}
)
target_include_directories (dream PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../dreamInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DREAM.cxx
  ${tarDIR}/DREAMvariables.cxx
  ${tarINC}/DREAM.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

