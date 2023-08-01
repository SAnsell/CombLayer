set (weightsSources
    CellWeight.cxx GridMesh.cxx ImportControl.cxx 
    MarkovProcess.cxx TempWeights.cxx WCellControl.cxx 
    WCells.cxx WeightCone.cxx WeightControl.cxx 
    WeightControlHelp.cxx weightManager.cxx WForm.cxx 
    WItem.cxx WWGControl.cxx WWG.cxx 
    WWGItem.cxx WWGWeight.cxx 
)

add_library (weights SHARED
  ${weightsSources}
)
target_include_directories (weights PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../weightsInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/CellWeight.cxx
  ${tarDIR}/GridMesh.cxx
  ${tarDIR}/ImportControl.cxx
  ${tarDIR}/MarkovProcess.cxx
  ${tarDIR}/TempWeights.cxx
  ${tarDIR}/WCellControl.cxx
  ${tarDIR}/WCells.cxx
  ${tarDIR}/WeightCone.cxx
  ${tarDIR}/WeightControl.cxx
  ${tarDIR}/WeightControlHelp.cxx
  ${tarDIR}/weightManager.cxx
  ${tarDIR}/WForm.cxx
  ${tarDIR}/WItem.cxx
  ${tarDIR}/WWGControl.cxx
  ${tarDIR}/WWG.cxx
  ${tarDIR}/WWGItem.cxx
  ${tarDIR}/WWGWeight.cxx
  ${tarINC}/CellWeight.h
  ${tarINC}/GridMesh.h
  ${tarINC}/ImportControl.h
  ${tarINC}/MarkovProcess.h
  ${tarINC}/TempWeights.h
  ${tarINC}/WCellControl.h
  ${tarINC}/WCells.h
  ${tarINC}/WeightCone.h
  ${tarINC}/WeightControl.h
  ${tarINC}/weightManager.h
  ${tarINC}/WForm.h
  ${tarINC}/WItem.h
  ${tarINC}/WWGControl.h
  ${tarINC}/WWG.h
  ${tarINC}/WWGItem.h
  ${tarINC}/WWGWeight.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

