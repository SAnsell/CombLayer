set (constructSources
    anglePortItem.cxx Aperture.cxx BasicFlightLine.cxx 
    BeamWindow.cxx Bellows.cxx BlankTube.cxx 
    CentredHoleShape.cxx ChopperPit.cxx ConicInfo.cxx 
    CornerPipe.cxx CrossPipe.cxx Cryostat.cxx 
    CrystalMount.cxx DomeConnector.cxx doublePortItem.cxx 
    FlangeDome.cxx FlangePlate.cxx FlightLine.cxx 
    GateValveCube.cxx GateValveCylinder.cxx generalConstruct.cxx 
    GeneralPipe.cxx gridUnit.cxx GroupOrigin.cxx 
    hexUnit.cxx HoleShape.cxx JawFlange.cxx 
    Jaws.cxx JawSet.cxx JawUnit.cxx 
    JawValveBase.cxx JawValveCube.cxx JawValveCylinder.cxx 
    JawValveTube.cxx LeadPipe.cxx LineShield.cxx 
    MultiChannel.cxx OffsetFlangePipe.cxx PinHole.cxx 
    PipeCollimator.cxx PipeTube.cxx portBuilder.cxx 
    portItem.cxx portSet.cxx PortTube.cxx 
    RingFlange.cxx RingSeal.cxx RotaryCollimator.cxx 
    SplitFlangePipe.cxx SupplyPipe.cxx TriangleShield.cxx 
    TubeDetBox.cxx tubeUnit.cxx VacuumBox.cxx 
    VacuumPipe.cxx VirtualTube.cxx voidCylVolume.cxx 
    VoidUnit.cxx WallCut.cxx Window.cxx 
    WindowPipe.cxx 
)

add_library (construct SHARED
  ${constructSources}
)
target_include_directories (construct PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../constructInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/anglePortItem.cxx
  ${tarDIR}/Aperture.cxx
  ${tarDIR}/BasicFlightLine.cxx
  ${tarDIR}/BeamWindow.cxx
  ${tarDIR}/Bellows.cxx
  ${tarDIR}/BlankTube.cxx
  ${tarDIR}/CentredHoleShape.cxx
  ${tarDIR}/ChopperPit.cxx
  ${tarDIR}/ConicInfo.cxx
  ${tarDIR}/CornerPipe.cxx
  ${tarDIR}/CrossPipe.cxx
  ${tarDIR}/Cryostat.cxx
  ${tarDIR}/CrystalMount.cxx
  ${tarDIR}/DomeConnector.cxx
  ${tarDIR}/doublePortItem.cxx
  ${tarDIR}/FlangeDome.cxx
  ${tarDIR}/FlangePlate.cxx
  ${tarDIR}/FlightLine.cxx
  ${tarDIR}/GateValveCube.cxx
  ${tarDIR}/GateValveCylinder.cxx
  ${tarDIR}/generalConstruct.cxx
  ${tarDIR}/GeneralPipe.cxx
  ${tarDIR}/gridUnit.cxx
  ${tarDIR}/GroupOrigin.cxx
  ${tarDIR}/hexUnit.cxx
  ${tarDIR}/HoleShape.cxx
  ${tarDIR}/JawFlange.cxx
  ${tarDIR}/Jaws.cxx
  ${tarDIR}/JawSet.cxx
  ${tarDIR}/JawUnit.cxx
  ${tarDIR}/JawValveBase.cxx
  ${tarDIR}/JawValveCube.cxx
  ${tarDIR}/JawValveCylinder.cxx
  ${tarDIR}/JawValveTube.cxx
  ${tarDIR}/LeadPipe.cxx
  ${tarDIR}/LineShield.cxx
  ${tarDIR}/MultiChannel.cxx
  ${tarDIR}/OffsetFlangePipe.cxx
  ${tarDIR}/PinHole.cxx
  ${tarDIR}/PipeCollimator.cxx
  ${tarDIR}/PipeTube.cxx
  ${tarDIR}/portBuilder.cxx
  ${tarDIR}/portItem.cxx
  ${tarDIR}/portSet.cxx
  ${tarDIR}/PortTube.cxx
  ${tarDIR}/RingFlange.cxx
  ${tarDIR}/RingSeal.cxx
  ${tarDIR}/RotaryCollimator.cxx
  ${tarDIR}/SplitFlangePipe.cxx
  ${tarDIR}/SupplyPipe.cxx
  ${tarDIR}/TriangleShield.cxx
  ${tarDIR}/TubeDetBox.cxx
  ${tarDIR}/tubeUnit.cxx
  ${tarDIR}/VacuumBox.cxx
  ${tarDIR}/VacuumPipe.cxx
  ${tarDIR}/VirtualTube.cxx
  ${tarDIR}/voidCylVolume.cxx
  ${tarDIR}/VoidUnit.cxx
  ${tarDIR}/WallCut.cxx
  ${tarDIR}/Window.cxx
  ${tarDIR}/WindowPipe.cxx
  ${tarINC}/anglePortItem.h
  ${tarINC}/Aperture.h
  ${tarINC}/BasicFlightLine.h
  ${tarINC}/BeamWindow.h
  ${tarINC}/Bellows.h
  ${tarINC}/BlankTube.h
  ${tarINC}/CentredHoleShape.h
  ${tarINC}/ChopperPit.h
  ${tarINC}/ConicInfo.h
  ${tarINC}/CornerPipe.h
  ${tarINC}/CrossPipe.h
  ${tarINC}/Cryostat.h
  ${tarINC}/CrystalMount.h
  ${tarINC}/DomeConnector.h
  ${tarINC}/doublePortItem.h
  ${tarINC}/FlangeDome.h
  ${tarINC}/FlangePlate.h
  ${tarINC}/FlightLine.h
  ${tarINC}/GateValveCube.h
  ${tarINC}/GateValveCylinder.h
  ${tarINC}/generalConstruct.h
  ${tarINC}/GeneralPipe.h
  ${tarINC}/gridUnit.h
  ${tarINC}/GroupOrigin.h
  ${tarINC}/hexUnit.h
  ${tarINC}/HoleShape.h
  ${tarINC}/JawFlange.h
  ${tarINC}/JawSet.h
  ${tarINC}/Jaws.h
  ${tarINC}/JawUnit.h
  ${tarINC}/JawValveBase.h
  ${tarINC}/JawValveCube.h
  ${tarINC}/JawValveCylinder.h
  ${tarINC}/JawValveTube.h
  ${tarINC}/LeadPipe.h
  ${tarINC}/LineShield.h
  ${tarINC}/MultiChannel.h
  ${tarINC}/OffsetFlangePipe.h
  ${tarINC}/PinHole.h
  ${tarINC}/PipeCollimator.h
  ${tarINC}/PipeTube.h
  ${tarINC}/portBuilder.h
  ${tarINC}/portItem.h
  ${tarINC}/portSet.h
  ${tarINC}/PortTube.h
  ${tarINC}/RingFlange.h
  ${tarINC}/RingSeal.h
  ${tarINC}/RotaryCollimator.h
  ${tarINC}/SplitFlangePipe.h
  ${tarINC}/SupplyPipe.h
  ${tarINC}/TriangleShield.h
  ${tarINC}/TubeDetBox.h
  ${tarINC}/tubeUnit.h
  ${tarINC}/VacuumBox.h
  ${tarINC}/VacuumPipe.h
  ${tarINC}/VirtualTube.h
  ${tarINC}/voidCylVolume.h
  ${tarINC}/VoidUnit.h
  ${tarINC}/WallCut.h
  ${tarINC}/Window.h
  ${tarINC}/WindowPipe.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

