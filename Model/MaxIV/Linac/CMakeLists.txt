set (LinacSources
    Wendi.cxx
    BeamBox.cxx BeamDivider.cxx BeamWing.cxx
    ButtonBPM.cxx CeramicGap.cxx CleaningMagnet.cxx
    CrossWayBlank.cxx CurveMagnet.cxx DipoleDIBMag.cxx
    EArrivalMon.cxx EBeamStop.cxx FlatPipe.cxx
    FMUndulator.cxx InjectionHall.cxx LBeamStop.cxx
    linacMagnetVariables.cxx linacVariables.cxx LObjectSupport.cxx
    LocalShielding3.cxx LocalShielding.cxx LowBeamBox.cxx
    LQuadF.cxx LQuadH.cxx LSexupole.cxx
    magnetVar.cxx MainBeamDump.cxx MultiPipe.cxx
    NBeamStop.cxx PrismaChamber.cxx Scrapper.cxx
    Segment10.cxx Segment11.cxx Segment12.cxx
    Segment13.cxx Segment14.cxx Segment15.cxx
    Segment16.cxx Segment17.cxx Segment18.cxx
    Segment19.cxx Segment1.cxx Segment20.cxx
    Segment21.cxx Segment23.cxx Segment24.cxx
    Segment25.cxx Segment26.cxx Segment27.cxx
    Segment28.cxx Segment29.cxx Segment2.cxx
    Segment30.cxx Segment31.cxx Segment32.cxx
    Segment33.cxx Segment34.cxx Segment35.cxx
    Segment36.cxx Segment37.cxx Segment38.cxx
    Segment39.cxx Segment3.cxx Segment40.cxx
    Segment41.cxx Segment42.cxx Segment43.cxx
    Segment44.cxx Segment45.cxx Segment46.cxx
    Segment47.cxx Segment48.cxx Segment49.cxx
    Segment4.cxx Segment5.cxx Segment6.cxx
    Segment7.cxx Segment8.cxx Segment9.cxx
    SoilRoof.cxx SPFCameraShield.cxx StriplineBPM.cxx
    subPipeUnit.cxx TDCBeamDump.cxx TDC.cxx
    TDCsegment.cxx TriGroup.cxx TriPipe.cxx
    TWCavity.cxx UndulatorVacuum.cxx YagScreen.cxx
    YagUnitBig.cxx YagUnit.cxx
)

add_library (Linac SHARED
  ${LinacSources}
)
target_include_directories (Linac PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LinacInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Wendi.cxx
  ${tarDIR}/BeamBox.cxx
  ${tarDIR}/BeamDivider.cxx
  ${tarDIR}/BeamWing.cxx
  ${tarDIR}/ButtonBPM.cxx
  ${tarDIR}/CeramicGap.cxx
  ${tarDIR}/CleaningMagnet.cxx
  ${tarDIR}/CrossWayBlank.cxx
  ${tarDIR}/CurveMagnet.cxx
  ${tarDIR}/DipoleDIBMag.cxx
  ${tarDIR}/EArrivalMon.cxx
  ${tarDIR}/EBeamStop.cxx
  ${tarDIR}/FlatPipe.cxx
  ${tarDIR}/FMUndulator.cxx
  ${tarDIR}/InjectionHall.cxx
  ${tarDIR}/LBeamStop.cxx
  ${tarDIR}/linacMagnetVariables.cxx
  ${tarDIR}/linacVariables.cxx
  ${tarDIR}/LObjectSupport.cxx
  ${tarDIR}/LocalShielding3.cxx
  ${tarDIR}/LocalShielding.cxx
  ${tarDIR}/LowBeamBox.cxx
  ${tarDIR}/LQuadF.cxx
  ${tarDIR}/LQuadH.cxx
  ${tarDIR}/LSexupole.cxx
  ${tarDIR}/magnetVar.cxx
  ${tarDIR}/MainBeamDump.cxx
  ${tarDIR}/MultiPipe.cxx
  ${tarDIR}/NBeamStop.cxx
  ${tarDIR}/PrismaChamber.cxx
  ${tarDIR}/Scrapper.cxx
  ${tarDIR}/Segment10.cxx
  ${tarDIR}/Segment11.cxx
  ${tarDIR}/Segment12.cxx
  ${tarDIR}/Segment13.cxx
  ${tarDIR}/Segment14.cxx
  ${tarDIR}/Segment15.cxx
  ${tarDIR}/Segment16.cxx
  ${tarDIR}/Segment17.cxx
  ${tarDIR}/Segment18.cxx
  ${tarDIR}/Segment19.cxx
  ${tarDIR}/Segment1.cxx
  ${tarDIR}/Segment20.cxx
  ${tarDIR}/Segment21.cxx
  ${tarDIR}/Segment23.cxx
  ${tarDIR}/Segment24.cxx
  ${tarDIR}/Segment25.cxx
  ${tarDIR}/Segment26.cxx
  ${tarDIR}/Segment27.cxx
  ${tarDIR}/Segment28.cxx
  ${tarDIR}/Segment29.cxx
  ${tarDIR}/Segment2.cxx
  ${tarDIR}/Segment30.cxx
  ${tarDIR}/Segment31.cxx
  ${tarDIR}/Segment32.cxx
  ${tarDIR}/Segment33.cxx
  ${tarDIR}/Segment34.cxx
  ${tarDIR}/Segment35.cxx
  ${tarDIR}/Segment36.cxx
  ${tarDIR}/Segment37.cxx
  ${tarDIR}/Segment38.cxx
  ${tarDIR}/Segment39.cxx
  ${tarDIR}/Segment3.cxx
  ${tarDIR}/Segment40.cxx
  ${tarDIR}/Segment41.cxx
  ${tarDIR}/Segment42.cxx
  ${tarDIR}/Segment43.cxx
  ${tarDIR}/Segment44.cxx
  ${tarDIR}/Segment45.cxx
  ${tarDIR}/Segment46.cxx
  ${tarDIR}/Segment47.cxx
  ${tarDIR}/Segment48.cxx
  ${tarDIR}/Segment49.cxx
  ${tarDIR}/Segment4.cxx
  ${tarDIR}/Segment5.cxx
  ${tarDIR}/Segment6.cxx
  ${tarDIR}/Segment7.cxx
  ${tarDIR}/Segment8.cxx
  ${tarDIR}/Segment9.cxx
  ${tarDIR}/SoilRoof.cxx
  ${tarDIR}/SPFCameraShield.cxx
  ${tarDIR}/StriplineBPM.cxx
  ${tarDIR}/subPipeUnit.cxx
  ${tarDIR}/TDCBeamDump.cxx
  ${tarDIR}/TDC.cxx
  ${tarDIR}/TDCsegment.cxx
  ${tarDIR}/TriGroup.cxx
  ${tarDIR}/TriPipe.cxx
  ${tarDIR}/TWCavity.cxx
  ${tarDIR}/UndulatorVacuum.cxx
  ${tarDIR}/YagScreen.cxx
  ${tarDIR}/YagUnitBig.cxx
  ${tarDIR}/YagUnit.cxx
  ${tarINC}/Wendi.h
  ${tarINC}/BeamBox.h
  ${tarINC}/BeamDivider.h
  ${tarINC}/BeamWing.h
  ${tarINC}/ButtonBPM.h
  ${tarINC}/CeramicGap.h
  ${tarINC}/CleaningMagnet.h
  ${tarINC}/CrossWayBlank.h
  ${tarINC}/CurveMagnet.h
  ${tarINC}/DipoleDIBMag.h
  ${tarINC}/EArrivalMon.h
  ${tarINC}/EBeamStop.h
  ${tarINC}/FlatPipe.h
  ${tarINC}/FMUndulator.h
  ${tarINC}/InjectionHall.h
  ${tarINC}/LBeamStop.h
  ${tarINC}/LObjectSupport.h
  ${tarINC}/LocalShielding3.h
  ${tarINC}/LocalShielding.h
  ${tarINC}/LowBeamBox.h
  ${tarINC}/LQuadF.h
  ${tarINC}/LQuadH.h
  ${tarINC}/LSexupole.h
  ${tarINC}/magnetVar.h
  ${tarINC}/MainBeamDump.h
  ${tarINC}/MultiPipe.h
  ${tarINC}/NBeamStop.h
  ${tarINC}/PrismaChamber.h
  ${tarINC}/Scrapper.h
  ${tarINC}/Segment10.h
  ${tarINC}/Segment11.h
  ${tarINC}/Segment12.h
  ${tarINC}/Segment13.h
  ${tarINC}/Segment14.h
  ${tarINC}/Segment15.h
  ${tarINC}/Segment16.h
  ${tarINC}/Segment17.h
  ${tarINC}/Segment18.h
  ${tarINC}/Segment19.h
  ${tarINC}/Segment1.h
  ${tarINC}/Segment20.h
  ${tarINC}/Segment21.h
  ${tarINC}/Segment23.h
  ${tarINC}/Segment24.h
  ${tarINC}/Segment25.h
  ${tarINC}/Segment26.h
  ${tarINC}/Segment27.h
  ${tarINC}/Segment28.h
  ${tarINC}/Segment29.h
  ${tarINC}/Segment2.h
  ${tarINC}/Segment30.h
  ${tarINC}/Segment31.h
  ${tarINC}/Segment32.h
  ${tarINC}/Segment33.h
  ${tarINC}/Segment34.h
  ${tarINC}/Segment35.h
  ${tarINC}/Segment36.h
  ${tarINC}/Segment37.h
  ${tarINC}/Segment38.h
  ${tarINC}/Segment39.h
  ${tarINC}/Segment3.h
  ${tarINC}/Segment40.h
  ${tarINC}/Segment41.h
  ${tarINC}/Segment42.h
  ${tarINC}/Segment43.h
  ${tarINC}/Segment44.h
  ${tarINC}/Segment45.h
  ${tarINC}/Segment46.h
  ${tarINC}/Segment47.h
  ${tarINC}/Segment48.h
  ${tarINC}/Segment49.h
  ${tarINC}/Segment4.h
  ${tarINC}/Segment5.h
  ${tarINC}/Segment6.h
  ${tarINC}/Segment7.h
  ${tarINC}/Segment8.h
  ${tarINC}/Segment9.h
  ${tarINC}/SoilRoof.h
  ${tarINC}/SPFCameraShield.h
  ${tarINC}/StriplineBPM.h
  ${tarINC}/subPipeUnit.h
  ${tarINC}/TDCBeamDump.h
  ${tarINC}/TDC.h
  ${tarINC}/TDCsegment.h
  ${tarINC}/TriGroup.h
  ${tarINC}/TriPipe.h
  ${tarINC}/TWCavity.h
  ${tarINC}/UndulatorVacuum.h
  ${tarINC}/YagScreen.h
  ${tarINC}/YagUnitBig.h
  ${tarINC}/YagUnit.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)
