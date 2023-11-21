set (flexpesSources
    FLEXPES.cxx flexpesFrontEnd.cxx flexpesVariables.cxx 
)

add_library (flexpes SHARED
  ${flexpesSources}
)
target_include_directories (flexpes PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../flexpesInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/FLEXPES.cxx
  ${tarDIR}/flexpesFrontEnd.cxx
  ${tarDIR}/flexpesVariables.cxx
  ${tarINC}/flexpesFrontEnd.h
  ${tarINC}/FLEXPES.h
  ${tarINC}/flexpesOpticsHut.h
  ${tarINC}/flexpesOpticsLine.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

