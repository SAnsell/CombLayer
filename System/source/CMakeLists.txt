set (sourceSources
    ActivationSource.cxx activeFluxPt.cxx activeUnit.cxx 
    BeamSource.cxx DSTerm.cxx FlukaSource.cxx 
    flukaSourceSelection.cxx GammaSource.cxx GaussBeamSource.cxx 
    KCodeSource.cxx LensSource.cxx ParabolicSource.cxx 
    phitsSourceSelection.cxx PointSource.cxx RectangleSource.cxx 
    SourceBase.cxx SourceCreate.cxx Source.cxx 
    sourceDataBase.cxx SourceSelector.cxx SrcData.cxx 
    SrcItem.cxx SurfNormSource.cxx SynchrotonBeam.cxx 
    TDCSource.cxx 
)

add_library (source SHARED
  ${sourceSources}
)
target_include_directories (source PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../sourceInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/ActivationSource.cxx
  ${tarDIR}/activeFluxPt.cxx
  ${tarDIR}/activeUnit.cxx
  ${tarDIR}/BeamSource.cxx
  ${tarDIR}/DSTerm.cxx
  ${tarDIR}/FlukaSource.cxx
  ${tarDIR}/flukaSourceSelection.cxx
  ${tarDIR}/GammaSource.cxx
  ${tarDIR}/GaussBeamSource.cxx
  ${tarDIR}/KCodeSource.cxx
  ${tarDIR}/LensSource.cxx
  ${tarDIR}/ParabolicSource.cxx
  ${tarDIR}/phitsSourceSelection.cxx
  ${tarDIR}/PointSource.cxx
  ${tarDIR}/RectangleSource.cxx
  ${tarDIR}/SourceBase.cxx
  ${tarDIR}/SourceCreate.cxx
  ${tarDIR}/Source.cxx
  ${tarDIR}/sourceDataBase.cxx
  ${tarDIR}/SourceSelector.cxx
  ${tarDIR}/SrcData.cxx
  ${tarDIR}/SrcItem.cxx
  ${tarDIR}/SurfNormSource.cxx
  ${tarDIR}/SynchrotonBeam.cxx
  ${tarDIR}/TDCSource.cxx
  ${tarINC}/ActivationSource.h
  ${tarINC}/activeFluxPt.h
  ${tarINC}/activeUnit.h
  ${tarINC}/BeamSource.h
  ${tarINC}/DSTerm.h
  ${tarINC}/FlukaSource.h
  ${tarINC}/flukaSourceSelector.h
  ${tarINC}/GammaSource.h
  ${tarINC}/GaussBeamSource.h
  ${tarINC}/KCodeSource.h
  ${tarINC}/LensSource.h
  ${tarINC}/ParabolicSource.h
  ${tarINC}/phitsSourceSelector.h
  ${tarINC}/PointSource.h
  ${tarINC}/RectangleSource.h
  ${tarINC}/SourceBase.h
  ${tarINC}/SourceCreate.h
  ${tarINC}/sourceDataBase.h
  ${tarINC}/Source.h
  ${tarINC}/SourceSelector.h
  ${tarINC}/SrcData.h
  ${tarINC}/SrcItem.h
  ${tarINC}/SurfNormSource.h
  ${tarINC}/SynchrotonBeam.h
  ${tarINC}/TDCSource.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

