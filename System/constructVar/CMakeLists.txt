set (constructVarSources
    BellowGenerator.cxx CornerPipeGenerator.cxx CrossGenerator.cxx
    DomeConnectorGenerator.cxx DoublePortItemGenerator.cxx
    FlangeDomeGenerator.cxx
    FlangePlateGenerator.cxx GateValveGenerator.cxx JawFlangeGenerator.cxx
    JawValveGenerator.cxx LeadPipeGenerator.cxx PipeGenerator.cxx
    PipeTubeGenerator.cxx PortItemGenerator.cxx PortTubeGenerator.cxx
    TorusUnitGenerator.cxx
    TubeDetBoxGenerator.cxx VacBoxGenerator.cxx
)

add_library (constructVar SHARED
  ${constructVarSources}
)
target_include_directories (constructVar PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../constructVarInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BellowGenerator.cxx
  ${tarDIR}/CornerPipeGenerator.cxx
  ${tarDIR}/CrossGenerator.cxx
  ${tarDIR}/DomeConnectorGenerator.cxx
  ${tarDIR}/DoublePortItemGenerator.cxx
  ${tarDIR}/FlangeDomeGenerator.cxx
  ${tarDIR}/FlangePlateGenerator.cxx
  ${tarDIR}/GateValveGenerator.cxx
  ${tarDIR}/JawFlangeGenerator.cxx
  ${tarDIR}/JawValveGenerator.cxx
  ${tarDIR}/LeadPipeGenerator.cxx
  ${tarDIR}/PipeGenerator.cxx
  ${tarDIR}/PipeTubeGenerator.cxx
  ${tarDIR}/PortItemGenerator.cxx
  ${tarDIR}/PortTubeGenerator.cxx
  ${tarDIR}/TorusUnitGenerator.cxx
  ${tarDIR}/TubeDetBoxGenerator.cxx
  ${tarDIR}/VacBoxGenerator.cxx
  ${tarINC}/BellowGenerator.h
  ${tarINC}/CFFlanges.h
  ${tarINC}/CornerPipeGenerator.h
  ${tarINC}/CrossGenerator.h
  ${tarINC}/DomeConnectorGenerator.h
  ${tarINC}/DoublePortItemGenerator.h
  ${tarINC}/FlangeDomeGenerator.h
  ${tarINC}/FlangePlateGenerator.h
  ${tarINC}/GateValveGenerator.h
  ${tarINC}/JawFlangeGenerator.h
  ${tarINC}/JawValveGenerator.h
  ${tarINC}/LeadPipeGenerator.h
  ${tarINC}/PipeGenerator.h
  ${tarINC}/PipeTubeGenerator.h
  ${tarINC}/PortItemGenerator.h
  ${tarINC}/PortTubeGenerator.h
  ${tarINC}/TorusUnitGenerator.h
  ${tarINC}/TubeDetBoxGenerator.h
  ${tarINC}/VacBoxGenerator.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)
