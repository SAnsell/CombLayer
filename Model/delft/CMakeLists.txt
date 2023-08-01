set (delftSources
    AirBoxElement.cxx BeamInsert.cxx beamSlot.cxx 
    BeamTube.cxx BeamTubeJoiner.cxx BeElement.cxx 
    BeFullBlock.cxx BeOElement.cxx BeSurround.cxx 
    ConeModerator.cxx ControlElement.cxx DefElement.cxx 
    delftH2Moderator.cxx delftVariables.cxx FlatModerator.cxx 
    FuelElement.cxx FuelLoad.cxx H2Groove.cxx 
    H2Vac.cxx HfElement.cxx IrradElement.cxx 
    makeDelft.cxx PressureVessel.cxx Rabbit.cxx 
    ReactorGrid.cxx ReactorLayout.cxx reactorTallyConstruct.cxx 
    RElement.cxx SpaceBlock.cxx SphereModerator.cxx 
    SwimingPool.cxx virtualMod.cxx 
)

add_library (delft SHARED
  ${delftSources}
)
target_include_directories (delft PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/delftInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../delftInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/AirBoxElement.cxx
  ${tarDIR}/BeamInsert.cxx
  ${tarDIR}/beamSlot.cxx
  ${tarDIR}/BeamTube.cxx
  ${tarDIR}/BeamTubeJoiner.cxx
  ${tarDIR}/BeElement.cxx
  ${tarDIR}/BeFullBlock.cxx
  ${tarDIR}/BeOElement.cxx
  ${tarDIR}/BeSurround.cxx
  ${tarDIR}/ConeModerator.cxx
  ${tarDIR}/ControlElement.cxx
  ${tarDIR}/DefElement.cxx
  ${tarDIR}/delftH2Moderator.cxx
  ${tarDIR}/delftVariables.cxx
  ${tarDIR}/FlatModerator.cxx
  ${tarDIR}/FuelElement.cxx
  ${tarDIR}/FuelLoad.cxx
  ${tarDIR}/H2Groove.cxx
  ${tarDIR}/H2Vac.cxx
  ${tarDIR}/HfElement.cxx
  ${tarDIR}/IrradElement.cxx
  ${tarDIR}/makeDelft.cxx
  ${tarDIR}/PressureVessel.cxx
  ${tarDIR}/Rabbit.cxx
  ${tarDIR}/ReactorGrid.cxx
  ${tarDIR}/ReactorLayout.cxx
  ${tarDIR}/reactorTallyConstruct.cxx
  ${tarDIR}/RElement.cxx
  ${tarDIR}/SpaceBlock.cxx
  ${tarDIR}/SphereModerator.cxx
  ${tarDIR}/SwimingPool.cxx
  ${tarDIR}/virtualMod.cxx
  ${tarINC}/AirBoxElement.h
  ${tarINC}/BeamInsert.h
  ${tarINC}/beamSlot.h
  ${tarINC}/BeamTube.h
  ${tarINC}/BeamTubeJoiner.h
  ${tarINC}/BeElement.h
  ${tarINC}/BeFullBlock.h
  ${tarINC}/BeOElement.h
  ${tarINC}/BeSurround.h
  ${tarINC}/ConeModerator.h
  ${tarINC}/ControlElement.h
  ${tarINC}/DefElement.h
  ${tarINC}/delftH2Moderator.h
  ${tarINC}/delftVariableSetup.h
  ${tarINC}/FlatModerator.h
  ${tarINC}/FuelElement.h
  ${tarINC}/FuelLoad.h
  ${tarINC}/H2Groove.h
  ${tarINC}/H2Vac.h
  ${tarINC}/HfElement.h
  ${tarINC}/IrradElement.h
  ${tarINC}/makeDelft.h
  ${tarINC}/PressureVessel.h
  ${tarINC}/Rabbit.h
  ${tarINC}/ReactorGrid.h
  ${tarINC}/reactorTallyConstruct.h
  ${tarINC}/RElement.h
  ${tarINC}/SpaceBlock.h
  ${tarINC}/SphereModerator.h
  ${tarINC}/SwimingPool.h
  ${tarINC}/virtualMod.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

