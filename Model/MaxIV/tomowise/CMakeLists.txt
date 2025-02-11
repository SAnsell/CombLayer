set (tomowiseSources
    TOMOWISE.cxx tomowiseExptLine.cxx
    tomowiseFrontEnd.cxx tomowiseOpticsLine.cxx tomowiseVariables.cxx
)

add_library (tomowise SHARED
  ${tomowiseSources}
)
target_include_directories (tomowise PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../tomowiseInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/TOMOWISE.cxx
  ${tarDIR}/tomowiseExptLine.cxx
  ${tarDIR}/tomowiseFrontEnd.cxx
  ${tarDIR}/tomowiseOpticsLine.cxx
  ${tarDIR}/tomowiseVariables.cxx
  ${tarINC}/TOMOWISE.h
  ${tarINC}/tomowiseOpticsLine.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)
