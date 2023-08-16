set (R3CommonSources
    ElectrometerBox.cxx EntryPipe.cxx EPCombine.cxx 
    EPContinue.cxx EPSeparator.cxx ExperimentalHutch.cxx 
    HalfElectronPipe.cxx MagnetM1.cxx MagnetU1.cxx 
    OpticsStepHutch.cxx R3Beamline.cxx R3ChokeChamber.cxx 
    R3ChokeInsert.cxx R3FrontEndCave.cxx R3FrontEnd.cxx 
    R3MagnetVariables.cxx R3Ring.cxx R3RingVariables.cxx 
)

add_library (R3Common SHARED
  ${R3CommonSources}
)
target_include_directories (R3Common PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../R3CommonInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/ElectrometerBox.cxx
  ${tarDIR}/EntryPipe.cxx
  ${tarDIR}/EPCombine.cxx
  ${tarDIR}/EPContinue.cxx
  ${tarDIR}/EPSeparator.cxx
  ${tarDIR}/ExperimentalHutch.cxx
  ${tarDIR}/HalfElectronPipe.cxx
  ${tarDIR}/MagnetM1.cxx
  ${tarDIR}/MagnetU1.cxx
  ${tarDIR}/OpticsStepHutch.cxx
  ${tarDIR}/R3Beamline.cxx
  ${tarDIR}/R3ChokeChamber.cxx
  ${tarDIR}/R3ChokeInsert.cxx
  ${tarDIR}/R3FrontEndCave.cxx
  ${tarDIR}/R3FrontEnd.cxx
  ${tarDIR}/R3MagnetVariables.cxx
  ${tarDIR}/R3Ring.cxx
  ${tarDIR}/R3RingVariables.cxx
  ${tarINC}/ElectrometerBox.h
  ${tarINC}/EntryPipe.h
  ${tarINC}/EPCombine.h
  ${tarINC}/EPContinue.h
  ${tarINC}/EPSeparator.h
  ${tarINC}/ExperimentalHutch.h
  ${tarINC}/HalfElectronPipe.h
  ${tarINC}/MagnetM1.h
  ${tarINC}/MagnetU1.h
  ${tarINC}/OpticsStepHutch.h
  ${tarINC}/R3Beamline.h
  ${tarINC}/R3ChokeChamber.h
  ${tarINC}/R3ChokeInsert.h
  ${tarINC}/R3FrontEndCave.h
  ${tarINC}/R3FrontEnd.h
  ${tarINC}/R3Ring.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

