set (lokiSources
    LOKI.cxx LOKIvariables.cxx VacTank.cxx 
)

add_library (loki SHARED
  ${lokiSources}
)
target_include_directories (loki PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../lokiInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/LOKI.cxx
  ${tarDIR}/LOKIvariables.cxx
  ${tarDIR}/VacTank.cxx
  ${tarINC}/LOKI.h
  ${tarINC}/VacTank.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

