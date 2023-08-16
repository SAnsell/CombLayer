set (cosaxsSources
    COSAXS.cxx cosaxsExptLine.cxx cosaxsFrontEnd.cxx 
    cosaxsOpticsLine.cxx cosaxsSampleArea.cxx cosaxsTubeAirBox.cxx 
    cosaxsTubeCable.cxx cosaxsTube.cxx cosaxsTubeNoseCone.cxx 
    cosaxsTubeStartPlate.cxx cosaxsTubeWAXSDetector.cxx cosaxsVariables.cxx 
)

add_library (cosaxs SHARED
  ${cosaxsSources}
)
target_include_directories (cosaxs PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../cosaxsInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/COSAXS.cxx
  ${tarDIR}/cosaxsExptLine.cxx
  ${tarDIR}/cosaxsFrontEnd.cxx
  ${tarDIR}/cosaxsOpticsLine.cxx
  ${tarDIR}/cosaxsSampleArea.cxx
  ${tarDIR}/cosaxsTubeAirBox.cxx
  ${tarDIR}/cosaxsTubeCable.cxx
  ${tarDIR}/cosaxsTube.cxx
  ${tarDIR}/cosaxsTubeNoseCone.cxx
  ${tarDIR}/cosaxsTubeStartPlate.cxx
  ${tarDIR}/cosaxsTubeWAXSDetector.cxx
  ${tarDIR}/cosaxsVariables.cxx
  ${tarINC}/cosaxsExptLine.h
  ${tarINC}/cosaxsFrontEnd.h
  ${tarINC}/COSAXS.h
  ${tarINC}/cosaxsOpticsLine.h
  ${tarINC}/cosaxsSampleArea.h
  ${tarINC}/cosaxsTubeAirBox.h
  ${tarINC}/cosaxsTubeCable.h
  ${tarINC}/cosaxsTube.h
  ${tarINC}/cosaxsTubeNoseCone.h
  ${tarINC}/cosaxsTubeStartPlate.h
  ${tarINC}/cosaxsTubeWAXSDetector.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

