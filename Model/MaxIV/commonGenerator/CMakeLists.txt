set (commonGeneratorSources
    AreaDetectorGenerator.cxx BeamBoxGenerator.cxx BeamDividerGenerator.cxx 
    BeamMountGenerator.cxx BeamPairGenerator.cxx BeamScrapperGenerator.cxx 
    BiPortGenerator.cxx BoxJawsGenerator.cxx BremBlockGenerator.cxx 
    BremCollGenerator.cxx BremMonoCollGenerator.cxx BremOpticsCollGenerator.cxx 
    BremTubeGenerator.cxx ButtonBPMGenerator.cxx CeramicGapGenerator.cxx 
    CleaningMagnetGenerator.cxx CollGenerator.cxx CollTubeGenerator.cxx 
    CollUnitGenerator.cxx ConnectorGenerator.cxx CooledScreenGenerator.cxx 
    CorrectorMagGenerator.cxx CRLTubeGenerator.cxx CrossWayGenerator.cxx 
    CurveMagGenerator.cxx CylGateValveGenerator.cxx DCMTankGenerator.cxx 
    DiffXIADP03Generator.cxx DipoleChamberGenerator.cxx DipoleDIBMagGenerator.cxx 
    DipoleExtractGenerator.cxx DipoleGenerator.cxx DipoleSndBendGenerator.cxx 
    EArrivalMonGenerator.cxx EBeamStopGenerator.cxx ElectroBoxGenerator.cxx 
    EPCombineGenerator.cxx EPContinueGenerator.cxx EPSeparaterGenerator.cxx 
    ExptHutGenerator.cxx FlangeMountGenerator.cxx FlatPipeGenerator.cxx 
    FMUndulatorGenerator.cxx GaugeGenerator.cxx GrateMonoBoxGenerator.cxx 
    GratingMonoGenerator.cxx GratingUnitGenerator.cxx HeatDumpGenerator.cxx 
    HPJawsGenerator.cxx IonGaugeGenerator.cxx IonPTubeGenerator.cxx 
    LBeamStopGenerator.cxx LinacQuadGenerator.cxx LinacSexuGenerator.cxx 
    LocalShieldingGenerator.cxx M1DetailGenerator.cxx  MagnetBlockGenerator.cxx 
    MagnetGenerator.cxx MagnetM1Generator.cxx MagnetU1Generator.cxx 
    MainBeamDumpGenerator.cxx MazeGenerator.cxx MirrorGenerator.cxx 
    MLMDetailGenerator.cxx MLMonoGenerator.cxx MonoBlockXstalsGenerator.cxx 
    MonoBoxGenerator.cxx MonoShutterGenerator.cxx MultiPipeGenerator.cxx 
    NBeamStopGenerator.cxx OctupoleGenerator.cxx OpticsHutGenerator.cxx 
    PipeShieldGenerator.cxx PortChicaneGenerator.cxx PreDipoleGenerator.cxx 
    PrismaChamberGenerator.cxx QuadrupoleGenerator.cxx QuadUnitGenerator.cxx 
    R3ChokeChamberGenerator.cxx RingDoorGenerator.cxx RoundShutterGenerator.cxx 
    ScrapperGenerator.cxx ScreenGenerator.cxx SexupoleGenerator.cxx 
    ShutterUnitGenerator.cxx SimpleChicaneGenerator.cxx SixPortGenerator.cxx 
    SPFCameraShieldGenerator.cxx SqrFMaskGenerator.cxx StriplineBPMGenerator.cxx 
    TableGenerator.cxx TankMonoVesselGenerator.cxx TDCBeamDumpGenerator.cxx 
    TriggerGenerator.cxx TriGroupGenerator.cxx TriPipeGenerator.cxx 
    TWCavityGenerator.cxx TwinPipeGenerator.cxx UndVacGenerator.cxx 
    ViewScreenGenerator.cxx WallLeadGenerator.cxx YagScreenGenerator.cxx 
    YagUnitBigGenerator.cxx YagUnitGenerator.cxx 
)

add_library (commonGenerator SHARED
  ${commonGeneratorSources}
)
target_include_directories (commonGenerator PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../commonGeneratorInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/AreaDetectorGenerator.cxx
  ${tarDIR}/BeamBoxGenerator.cxx
  ${tarDIR}/BeamDividerGenerator.cxx
  ${tarDIR}/BeamMountGenerator.cxx
  ${tarDIR}/BeamPairGenerator.cxx
  ${tarDIR}/BeamScrapperGenerator.cxx
  ${tarDIR}/BiPortGenerator.cxx
  ${tarDIR}/BoxJawsGenerator.cxx
  ${tarDIR}/BremBlockGenerator.cxx
  ${tarDIR}/BremCollGenerator.cxx
  ${tarDIR}/BremMonoCollGenerator.cxx
  ${tarDIR}/BremOpticsCollGenerator.cxx
  ${tarDIR}/BremTubeGenerator.cxx
  ${tarDIR}/ButtonBPMGenerator.cxx
  ${tarDIR}/CeramicGapGenerator.cxx
  ${tarDIR}/CleaningMagnetGenerator.cxx
  ${tarDIR}/CollGenerator.cxx
  ${tarDIR}/CollTubeGenerator.cxx
  ${tarDIR}/CollUnitGenerator.cxx
  ${tarDIR}/ConnectorGenerator.cxx
  ${tarDIR}/CooledScreenGenerator.cxx
  ${tarDIR}/CorrectorMagGenerator.cxx
  ${tarDIR}/CRLTubeGenerator.cxx
  ${tarDIR}/CrossWayGenerator.cxx
  ${tarDIR}/CurveMagGenerator.cxx
  ${tarDIR}/CylGateValveGenerator.cxx
  ${tarDIR}/DCMTankGenerator.cxx
  ${tarDIR}/DiffXIADP03Generator.cxx
  ${tarDIR}/DipoleChamberGenerator.cxx
  ${tarDIR}/DipoleDIBMagGenerator.cxx
  ${tarDIR}/DipoleExtractGenerator.cxx
  ${tarDIR}/DipoleGenerator.cxx
  ${tarDIR}/DipoleSndBendGenerator.cxx
  ${tarDIR}/EArrivalMonGenerator.cxx
  ${tarDIR}/EBeamStopGenerator.cxx
  ${tarDIR}/ElectroBoxGenerator.cxx
  ${tarDIR}/EPCombineGenerator.cxx
  ${tarDIR}/EPContinueGenerator.cxx
  ${tarDIR}/EPSeparaterGenerator.cxx
  ${tarDIR}/ExptHutGenerator.cxx
  ${tarDIR}/FlangeMountGenerator.cxx
  ${tarDIR}/FlatPipeGenerator.cxx
  ${tarDIR}/FMUndulatorGenerator.cxx
  ${tarDIR}/GaugeGenerator.cxx
  ${tarDIR}/GrateMonoBoxGenerator.cxx
  ${tarDIR}/GratingMonoGenerator.cxx
  ${tarDIR}/GratingUnitGenerator.cxx
  ${tarDIR}/HeatDumpGenerator.cxx
  ${tarDIR}/HPJawsGenerator.cxx
  ${tarDIR}/IonGaugeGenerator.cxx
  ${tarDIR}/IonPTubeGenerator.cxx
  ${tarDIR}/LBeamStopGenerator.cxx
  ${tarDIR}/LinacQuadGenerator.cxx
  ${tarDIR}/LinacSexuGenerator.cxx
  ${tarDIR}/LocalShieldingGenerator.cxx
  ${tarDIR}/M1DetailGenerator.cxx
  ${tarDIR}/MagnetBlockGenerator.cxx
  ${tarDIR}/MagnetGenerator.cxx
  ${tarDIR}/MagnetM1Generator.cxx
  ${tarDIR}/MagnetU1Generator.cxx
  ${tarDIR}/MainBeamDumpGenerator.cxx
  ${tarDIR}/MazeGenerator.cxx
  ${tarDIR}/MirrorGenerator.cxx
  ${tarDIR}/MLMDetailGenerator.cxx
  ${tarDIR}/MLMonoGenerator.cxx
  ${tarDIR}/MonoBlockXstalsGenerator.cxx
  ${tarDIR}/MonoBoxGenerator.cxx
  ${tarDIR}/MonoShutterGenerator.cxx
  ${tarDIR}/MultiPipeGenerator.cxx
  ${tarDIR}/NBeamStopGenerator.cxx
  ${tarDIR}/OctupoleGenerator.cxx
  ${tarDIR}/OpticsHutGenerator.cxx
  ${tarDIR}/PipeShieldGenerator.cxx
  ${tarDIR}/PortChicaneGenerator.cxx
  ${tarDIR}/PreDipoleGenerator.cxx
  ${tarDIR}/PrismaChamberGenerator.cxx
  ${tarDIR}/QuadrupoleGenerator.cxx
  ${tarDIR}/QuadUnitGenerator.cxx
  ${tarDIR}/R3ChokeChamberGenerator.cxx
  ${tarDIR}/RingDoorGenerator.cxx
  ${tarDIR}/RoundShutterGenerator.cxx
  ${tarDIR}/ScrapperGenerator.cxx
  ${tarDIR}/ScreenGenerator.cxx
  ${tarDIR}/SexupoleGenerator.cxx
  ${tarDIR}/ShutterUnitGenerator.cxx
  ${tarDIR}/SimpleChicaneGenerator.cxx
  ${tarDIR}/SixPortGenerator.cxx
  ${tarDIR}/SPFCameraShieldGenerator.cxx
  ${tarDIR}/SqrFMaskGenerator.cxx
  ${tarDIR}/StriplineBPMGenerator.cxx
  ${tarDIR}/TableGenerator.cxx
  ${tarDIR}/TankMonoVesselGenerator.cxx
  ${tarDIR}/TDCBeamDumpGenerator.cxx
  ${tarDIR}/TriggerGenerator.cxx
  ${tarDIR}/TriGroupGenerator.cxx
  ${tarDIR}/TriPipeGenerator.cxx
  ${tarDIR}/TWCavityGenerator.cxx
  ${tarDIR}/TwinPipeGenerator.cxx
  ${tarDIR}/UndVacGenerator.cxx
  ${tarDIR}/ViewScreenGenerator.cxx
  ${tarDIR}/WallLeadGenerator.cxx
  ${tarDIR}/YagScreenGenerator.cxx
  ${tarDIR}/YagUnitBigGenerator.cxx
  ${tarDIR}/YagUnitGenerator.cxx
  ${tarINC}/AreaDetectorGenerator.h
  ${tarINC}/BeamBoxGenerator.h
  ${tarINC}/BeamDividerGenerator.h
  ${tarINC}/BeamMountGenerator.h
  ${tarINC}/BeamPairGenerator.h
  ${tarINC}/BeamScrapperGenerator.h
  ${tarINC}/BiPortGenerator.h
  ${tarINC}/BoxJawsGenerator.h
  ${tarINC}/BremBlockGenerator.h
  ${tarINC}/BremCollGenerator.h
  ${tarINC}/BremMonoCollGenerator.h
  ${tarINC}/BremOpticsCollGenerator.h
  ${tarINC}/BremTubeGenerator.h
  ${tarINC}/ButtonBPMGenerator.h
  ${tarINC}/CeramicGapGenerator.h
  ${tarINC}/CleaningMagnetGenerator.h
  ${tarINC}/CollGenerator.h
  ${tarINC}/CollTubeGenerator.h
  ${tarINC}/CollUnitGenerator.h
  ${tarINC}/ConnectorGenerator.h
  ${tarINC}/CooledScreenGenerator.h
  ${tarINC}/CorrectorMagGenerator.h
  ${tarINC}/CRLTubeGenerator.h
  ${tarINC}/CrossWayGenerator.h
  ${tarINC}/CurveMagGenerator.h
  ${tarINC}/CylGateValveGenerator.h
  ${tarINC}/DCMTankGenerator.h
  ${tarINC}/DiffXIADP03Generator.h
  ${tarINC}/DipoleChamberGenerator.h
  ${tarINC}/DipoleDIBMagGenerator.h
  ${tarINC}/DipoleExtractGenerator.h
  ${tarINC}/DipoleGenerator.h
  ${tarINC}/DipoleSndBendGenerator.h
  ${tarINC}/EArrivalMonGenerator.h
  ${tarINC}/EBeamStopGenerator.h
  ${tarINC}/ElectroBoxGenerator.h
  ${tarINC}/EPCombineGenerator.h
  ${tarINC}/EPContinueGenerator.h
  ${tarINC}/EPSeparatorGenerator.h
  ${tarINC}/ExptHutGenerator.h
  ${tarINC}/FlangeMountGenerator.h
  ${tarINC}/FlatPipeGenerator.h
  ${tarINC}/FMUndulatorGenerator.h
  ${tarINC}/GaugeGenerator.h
  ${tarINC}/GrateMonoBoxGenerator.h
  ${tarINC}/GratingMonoGenerator.h
  ${tarINC}/GratingUnitGenerator.h
  ${tarINC}/HeatDumpGenerator.h
  ${tarINC}/HPJawsGenerator.h
  ${tarINC}/IonGaugeGenerator.h
  ${tarINC}/IonPTubeGenerator.h
  ${tarINC}/LBeamStopGenerator.h
  ${tarINC}/LinacQuadGenerator.h
  ${tarINC}/LinacSexuGenerator.h
  ${tarINC}/LocalShieldingGenerator.h
  ${tarINC}/M1DetailGenerator.h
  ${tarINC}/MagnetBlockGenerator.h
  ${tarINC}/MagnetGenerator.h
  ${tarINC}/MagnetM1Generator.h
  ${tarINC}/MagnetU1Generator.h
  ${tarINC}/MainBeamDumpGenerator.h
  ${tarINC}/MazeGenerator.h
  ${tarINC}/MirrorGenerator.h
  ${tarINC}/MLMDetailGenerator.h
  ${tarINC}/MLMonoGenerator.h
  ${tarINC}/MonoBlockXstalsGenerator.h
  ${tarINC}/MonoBoxGenerator.h
  ${tarINC}/MonoShutterGenerator.h
  ${tarINC}/MultiPipeGenerator.h
  ${tarINC}/NBeamStopGenerator.h
  ${tarINC}/OctupoleGenerator.h
  ${tarINC}/OpticsHutGenerator.h
  ${tarINC}/PipeShieldGenerator.h
  ${tarINC}/PortChicaneGenerator.h
  ${tarINC}/PreDipoleGenerator.h
  ${tarINC}/PrismaChamberGenerator.h
  ${tarINC}/QuadrupoleGenerator.h
  ${tarINC}/QuadUnitGenerator.h
  ${tarINC}/R3ChokeChamberGenerator.h
  ${tarINC}/RingDoorGenerator.h
  ${tarINC}/RoundShutterGenerator.h
  ${tarINC}/ScrapperGenerator.h
  ${tarINC}/ScreenGenerator.h
  ${tarINC}/SexupoleGenerator.h
  ${tarINC}/ShutterUnitGenerator.h
  ${tarINC}/SimpleChicaneGenerator.h
  ${tarINC}/SixPortGenerator.h
  ${tarINC}/SPFCameraShieldGenerator.h
  ${tarINC}/SqrFMaskGenerator.h
  ${tarINC}/StriplineBPMGenerator.h
  ${tarINC}/TableGenerator.h
  ${tarINC}/TankMonoVesselGenerator.h
  ${tarINC}/TDCBeamDumpGenerator.h
  ${tarINC}/TriggerGenerator.h
  ${tarINC}/TriGroupGenerator.h
  ${tarINC}/TriPipeGenerator.h
  ${tarINC}/TWCavityGenerator.h
  ${tarINC}/TwinPipeGenerator.h
  ${tarINC}/UndVacGenerator.h
  ${tarINC}/ViewScreenGenerator.h
  ${tarINC}/WallLeadGenerator.h
  ${tarINC}/YagScreenGenerator.h
  ${tarINC}/YagUnitBigGenerator.h
  ${tarINC}/YagUnitGenerator.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

