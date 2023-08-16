set (vorSources
    DetectorTank.cxx DHut.cxx VOR.cxx 
    VORvariables.cxx 
)

add_library (vor SHARED
  ${vorSources}
)
target_include_directories (vor PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vorInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DetectorTank.cxx
  ${tarDIR}/DHut.cxx
  ${tarDIR}/VOR.cxx
  ${tarDIR}/VORvariables.cxx
  ${tarINC}/DetectorTank.h
  ${tarINC}/DHut.h
  ${tarINC}/VOR.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

