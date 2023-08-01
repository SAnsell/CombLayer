set (saxsSources
    AreaPlate.cxx Capillary.cxx DefUnitsSAXS.cxx 
    makeSAXS.cxx saxsVariables.cxx 
)

add_library (saxs SHARED
  ${saxsSources}
)
target_include_directories (saxs PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/saxsInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../saxsInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/AreaPlate.cxx
  ${tarDIR}/Capillary.cxx
  ${tarDIR}/DefUnitsSAXS.cxx
  ${tarDIR}/makeSAXS.cxx
  ${tarDIR}/saxsVariables.cxx
  ${tarINC}/AreaPlate.h
  ${tarINC}/Capillary.h
  ${tarINC}/DefUnitsSAXS.h
  ${tarINC}/makeSAXS.h
  ${tarINC}/saxsVariableSetup.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

