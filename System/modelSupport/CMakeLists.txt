set (modelSupportSources
    BoxLine.cxx boxUnit.cxx boxValues.cxx 
    createDivide.cxx defaultConfig.cxx DivideGrid.cxx 
    generateSurf.cxx LineTrack.cxx LineUnit.cxx masterWrite.cxx 
    MaterialSupport.cxx MaterialUpdate.cxx mergeDist.cxx 
    ObjectAddition.cxx objectRegister.cxx ObjectTrackAct.cxx 
    ObjectTrackPlane.cxx ObjectTrackPoint.cxx ObjSurfMap.cxx 
    ObjTrackItem.cxx PipeLine.cxx pipeSupport.cxx 
    pipeUnit.cxx ReportSelector.cxx SimImportance.cxx 
    SimInput.cxx SpecialSurf.cxx 
    surfDBase.cxx surfDIter.cxx surfEqual.cxx 
    surfExpand.cxx surfRegister.cxx vertexCalc.cxx 
    Volumes.cxx volUnit.cxx Wrapper.cxx 
)

add_library (modelSupport SHARED
  ${modelSupportSources}
)
target_include_directories (modelSupport PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../modelSupportInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BoxLine.cxx
  ${tarDIR}/boxUnit.cxx
  ${tarDIR}/boxValues.cxx
  ${tarDIR}/createDivide.cxx
  ${tarDIR}/defaultConfig.cxx
  ${tarDIR}/DivideGrid.cxx
  ${tarDIR}/generateSurf.cxx
  ${tarDIR}/LineTrack.cxx
  ${tarDIR}/LineUnit.cxx
  ${tarDIR}/masterWrite.cxx
  ${tarDIR}/MaterialSupport.cxx
  ${tarDIR}/MaterialUpdate.cxx
  ${tarDIR}/mergeDist.cxx
  ${tarDIR}/ObjectAddition.cxx
  ${tarDIR}/objectRegister.cxx
  ${tarDIR}/ObjectTrackAct.cxx
  ${tarDIR}/ObjectTrackPlane.cxx
  ${tarDIR}/ObjectTrackPoint.cxx
  ${tarDIR}/ObjSurfMap.cxx
  ${tarDIR}/ObjTrackItem.cxx
  ${tarDIR}/PipeLine.cxx
  ${tarDIR}/pipeSupport.cxx
  ${tarDIR}/pipeUnit.cxx
  ${tarDIR}/ReportSelector.cxx
  ${tarDIR}/SimImportance.cxx
  ${tarDIR}/SimInput.cxx
  ${tarDIR}/SpecialSurf.cxx
  ${tarDIR}/surfDBase.cxx
  ${tarDIR}/surfDIter.cxx
  ${tarDIR}/surfEqual.cxx
  ${tarDIR}/surfExpand.cxx
  ${tarDIR}/surfRegister.cxx
  ${tarDIR}/vertexCalc.cxx
  ${tarDIR}/Volumes.cxx
  ${tarDIR}/volUnit.cxx
  ${tarDIR}/Wrapper.cxx
  ${tarINC}/BoxLine.h
  ${tarINC}/boxUnit.h
  ${tarINC}/boxValues.h
  ${tarINC}/createDivide.h
  ${tarINC}/defaultConfig.h
  ${tarINC}/DivideGrid.h
  ${tarINC}/generateSurf.h
  ${tarINC}/LineTrack.h
  ${tarINC}/LineUnit.h
  ${tarINC}/masterWrite.h
  ${tarINC}/MaterialSupport.h
  ${tarINC}/MaterialUpdate.h
  ${tarINC}/mcnpProcess.h
  ${tarINC}/mergeDist.h
  ${tarINC}/ObjectAddition.h
  ${tarINC}/objectRegister.h
  ${tarINC}/ObjectTrackAct.h
  ${tarINC}/ObjectTrackPlane.h
  ${tarINC}/ObjectTrackPoint.h
  ${tarINC}/ObjSurfMap.h
  ${tarINC}/ObjTrackItem.h
  ${tarINC}/pairItem.h
  ${tarINC}/PipeLine.h
  ${tarINC}/pipeSupport.h
  ${tarINC}/pipeUnit.h
  ${tarINC}/ReportSelector.h
  ${tarINC}/SimInput.h
  ${tarINC}/SpecialSurf.h
  ${tarINC}/surfDBase.h
  ${tarINC}/surfDIter.h
  ${tarINC}/surfEqual.h
  ${tarINC}/surfExpand.h
  ${tarINC}/surfRegister.h
  ${tarINC}/vertexCalc.h
  ${tarINC}/Volumes.h
  ${tarINC}/volUnit.h
  ${tarINC}/Wrapper.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

