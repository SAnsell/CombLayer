set (estiaSources
    ESTIA.cxx ESTIAvariables.cxx 
)

add_library (estia SHARED
  ${estiaSources}
)
target_include_directories (estia PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../estiaInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/ESTIA.cxx
  ${tarDIR}/ESTIAvariables.cxx
  ${tarINC}/ESTIA.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

