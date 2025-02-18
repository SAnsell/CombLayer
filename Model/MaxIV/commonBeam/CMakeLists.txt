set (commonBeamSources
    AreaDetector.cxx BeamMount.cxx BeamPair.cxx
    BeamScrapper.cxx BlockStand.cxx BoxJaws.cxx
    BremBlock.cxx BremColl.cxx BremMonoColl.cxx
    BremTube.cxx CollTube.cxx CollUnit.cxx
    ConnectorTube.cxx CooledScreen2.cxx CooledScreen.cxx
    CorrectorMag.cxx CRLTube.cxx CrossWayTube.cxx
    CylGateValve.cxx DCMTank.cxx DiffPumpXIADP03.cxx
    Dipole.cxx Duct.cxx FilterHolder.cxx
    FlangeMount.cxx forkHoles.cxx FourPortTube.cxx
    GaugeTube.cxx GrateHolder.cxx GrateMonoBox.cxx
    GratingMono.cxx GratingUnit.cxx HeatDump.cxx
    HPJaws.cxx IonGauge.cxx IonPumpTube.cxx
    LCollimator.cxx Maze.cxx Mirror.cxx
    MLMono.cxx MonoBeamStop.cxx MonoBlockXstals.cxx
    MonoBox.cxx MonoCrystals.cxx MonoShutter.cxx MovableSafetyMask.cxx
    Octupole.cxx OpticsHutch.cxx PipeShield.cxx
    PortChicane.cxx Quadrupole.cxx RingDoor.cxx
    RoundMonoShutter.cxx Sexupole.cxx ShutterUnit.cxx
    SideShield.cxx SimpleChicane.cxx SixPortTube.cxx
    SqrCollimator.cxx SquareFMask.cxx Table.cxx
    TankMonoVessel.cxx TriggerTube.cxx TwinPipe.cxx
    Undulator.cxx UTubePipe.cxx ViewScreenTube.cxx
    WallLead.cxx XrayInputs.cxx
)

add_library (commonBeam SHARED
  ${commonBeamSources}
)
target_include_directories (commonBeam PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../commonBeamInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/AreaDetector.cxx
  ${tarDIR}/BeamMount.cxx
  ${tarDIR}/BeamPair.cxx
  ${tarDIR}/BeamScrapper.cxx
  ${tarDIR}/BlockStand.cxx
  ${tarDIR}/BoxJaws.cxx
  ${tarDIR}/BremBlock.cxx
  ${tarDIR}/BremColl.cxx
  ${tarDIR}/BremMonoColl.cxx
  ${tarDIR}/BremTube.cxx
  ${tarDIR}/CollTube.cxx
  ${tarDIR}/CollUnit.cxx
  ${tarDIR}/ConnectorTube.cxx
  ${tarDIR}/CooledScreen2.cxx
  ${tarDIR}/CooledScreen.cxx
  ${tarDIR}/CorrectorMag.cxx
  ${tarDIR}/CRLTube.cxx
  ${tarDIR}/CrossWayTube.cxx
  ${tarDIR}/CylGateValve.cxx
  ${tarDIR}/DCMTank.cxx
  ${tarDIR}/DiffPumpXIADP03.cxx
  ${tarDIR}/Dipole.cxx
  ${tarDIR}/Duct.cxx
  ${tarDIR}/FilterHolder.cxx
  ${tarDIR}/FlangeMount.cxx
  ${tarDIR}/forkHoles.cxx
  ${tarDIR}/FourPortTube.cxx
  ${tarDIR}/GaugeTube.cxx
  ${tarDIR}/GrateHolder.cxx
  ${tarDIR}/GrateMonoBox.cxx
  ${tarDIR}/GratingMono.cxx
  ${tarDIR}/GratingUnit.cxx
  ${tarDIR}/HeatDump.cxx
  ${tarDIR}/HPJaws.cxx
  ${tarDIR}/IonGauge.cxx
  ${tarDIR}/IonPumpTube.cxx
  ${tarDIR}/LCollimator.cxx
  ${tarDIR}/Maze.cxx
  ${tarDIR}/Mirror.cxx
  ${tarDIR}/MLMono.cxx
  ${tarDIR}/MonoBeamStop.cxx
  ${tarDIR}/MonoBlockXstals.cxx
  ${tarDIR}/MonoBox.cxx
  ${tarDIR}/MonoCrystals.cxx
  ${tarDIR}/MonoShutter.cxx
  ${tarDir}/MovableSafetyMask.cxx
  ${tarDIR}/Octupole.cxx
  ${tarDIR}/OpticsHutch.cxx
  ${tarDIR}/PipeShield.cxx
  ${tarDIR}/PortChicane.cxx
  ${tarDIR}/Quadrupole.cxx
  ${tarDIR}/RingDoor.cxx
  ${tarDIR}/RoundMonoShutter.cxx
  ${tarDIR}/Sexupole.cxx
  ${tarDIR}/ShutterUnit.cxx
  ${tarDIR}/SideShield.cxx
  ${tarDIR}/SimpleChicane.cxx
  ${tarDIR}/SixPortTube.cxx
  ${tarDIR}/SqrCollimator.cxx
  ${tarDIR}/SquareFMask.cxx
  ${tarDIR}/Table.cxx
  ${tarDIR}/TankMonoVessel.cxx
  ${tarDIR}/TriggerTube.cxx
  ${tarDIR}/TwinPipe.cxx
  ${tarDIR}/Undulator.cxx
  ${tarDIR}/UTubePipe.cxx
  ${tarDIR}/ViewScreenTube.cxx
  ${tarDIR}/WallLead.cxx
  ${tarDIR}/XrayInputs.cxx
  ${tarINC}/AreaDetector.h
  ${tarINC}/BeamMount.h
  ${tarINC}/BeamPair.h
  ${tarINC}/BeamScrapper.h
  ${tarINC}/BlockStand.h
  ${tarINC}/BoxJaws.h
  ${tarINC}/BremBlock.h
  ${tarINC}/BremColl.h
  ${tarINC}/BremMonoColl.h
  ${tarINC}/BremTube.h
  ${tarINC}/CollTube.h
  ${tarINC}/CollUnit.h
  ${tarINC}/ConnectorTube.h
  ${tarINC}/CooledScreen2.h
  ${tarINC}/CooledScreen.h
  ${tarINC}/CorrectorMag.h
  ${tarINC}/CRLTube.h
  ${tarINC}/CrossWayTube.h
  ${tarINC}/CylGateValve.h
  ${tarINC}/DCMTank.h
  ${tarINC}/DiffPumpXIADP03.h
  ${tarINC}/Dipole.h
  ${tarINC}/Duct.h
  ${tarINC}/FilterHolder.h
  ${tarINC}/FlangeMount.h
  ${tarINC}/forkHoles.h
  ${tarINC}/FourPortTube.h
  ${tarINC}/GaugeTube.h
  ${tarINC}/GrateHolder.h
  ${tarINC}/GrateMonoBox.h
  ${tarINC}/GratingMono.h
  ${tarINC}/GratingUnit.h
  ${tarINC}/HeatDump.h
  ${tarINC}/HPJaws.h
  ${tarINC}/IonGauge.h
  ${tarINC}/IonPumpTube.h
  ${tarINC}/LCollimator.h
  ${tarINC}/Maze.h
  ${tarINC}/Mirror.h
  ${tarINC}/MLMono.h
  ${tarINC}/MonoBeamStop.h
  ${tarINC}/MonoBlockXstals.h
  ${tarINC}/MonoBox.h
  ${tarINC}/MonoCrystals.h
  ${tarINC}/MonoShutter.h
  ${tarINC}/MovableSafetyMask.h
  ${tarINC}/Octupole.h
  ${tarINC}/OpticsHutch.h
  ${tarINC}/PipeShield.h
  ${tarINC}/PortChicane.h
  ${tarINC}/Quadrupole.h
  ${tarINC}/RingDoor.h
  ${tarINC}/RoundMonoShutter.h
  ${tarINC}/Sexupole.h
  ${tarINC}/ShutterUnit.h
  ${tarINC}/SideShield.h
  ${tarINC}/SimpleChicane.h
  ${tarINC}/SixPortTube.h
  ${tarINC}/SqrCollimator.h
  ${tarINC}/SquareFMask.h
  ${tarINC}/Table.h
  ${tarINC}/TankMonoVessel.h
  ${tarINC}/TriggerTube.h
  ${tarINC}/TwinPipe.h
  ${tarINC}/Undulator.h
  ${tarINC}/UTubePipe.h
  ${tarINC}/ViewScreenTube.h
  ${tarINC}/WallLead.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)
