set (R1CommonSources
    DipoleChamber.cxx DipoleExtract.cxx DipoleSndBend.cxx 
    MagnetBlock.cxx QuadUnit.cxx R1Beamline.cxx 
    R1FrontEnd.cxx R1Ring.cxx R1RingVariables.cxx 
)

add_library (R1Common SHARED
  ${R1CommonSources}
)
target_include_directories (R1Common PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../R1CommonInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DipoleChamber.cxx
  ${tarDIR}/DipoleExtract.cxx
  ${tarDIR}/DipoleSndBend.cxx
  ${tarDIR}/MagnetBlock.cxx
  ${tarDIR}/QuadUnit.cxx
  ${tarDIR}/R1Beamline.cxx
  ${tarDIR}/R1FrontEnd.cxx
  ${tarDIR}/R1Ring.cxx
  ${tarDIR}/R1RingVariables.cxx
  ${tarINC}/DipoleChamber.h
  ${tarINC}/DipoleExtract.h
  ${tarINC}/DipoleSndBend.h
  ${tarINC}/MagnetBlock.h
  ${tarINC}/QuadUnit.h
  ${tarINC}/R1Beamline.h
  ${tarINC}/R1FrontEnd.h
  ${tarINC}/R1Ring.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

