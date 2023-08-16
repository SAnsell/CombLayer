set (heimdalSources
    HEIMDAL.cxx HEIMDALvariables.cxx 
)

add_library (heimdal SHARED
  ${heimdalSources}
)
target_include_directories (heimdal PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../heimdalInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/HEIMDAL.cxx
  ${tarDIR}/HEIMDALvariables.cxx
  ${tarINC}/HEIMDAL.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

