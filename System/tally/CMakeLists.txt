set (tallySources
    cellFluxTally.cxx fissionConstruct.cxx fissionTally.cxx 
    fluxConstruct.cxx fmeshConstruct.cxx fmeshTally.cxx 
    gridConstruct.cxx heatConstruct.cxx heatTally.cxx 
    itemConstruct.cxx PDControl.cxx pointConstruct.cxx 
    pointTally.cxx sswConstruct.cxx sswTally.cxx 
    surfaceConstruct.cxx surfaceTally.cxx TallyBuilder.cxx 
    TallyCreate.cxx Tally.cxx TallySelector.cxx 
    tmeshConstruct.cxx tmeshTally.cxx 
)

add_library (tally SHARED
  ${tallySources}
)
target_include_directories (tally PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../tallyInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/cellFluxTally.cxx
  ${tarDIR}/fissionConstruct.cxx
  ${tarDIR}/fissionTally.cxx
  ${tarDIR}/fluxConstruct.cxx
  ${tarDIR}/fmeshConstruct.cxx
  ${tarDIR}/fmeshTally.cxx
  ${tarDIR}/gridConstruct.cxx
  ${tarDIR}/heatConstruct.cxx
  ${tarDIR}/heatTally.cxx
  ${tarDIR}/itemConstruct.cxx
  ${tarDIR}/PDControl.cxx
  ${tarDIR}/pointConstruct.cxx
  ${tarDIR}/pointTally.cxx
  ${tarDIR}/sswConstruct.cxx
  ${tarDIR}/sswTally.cxx
  ${tarDIR}/surfaceConstruct.cxx
  ${tarDIR}/surfaceTally.cxx
  ${tarDIR}/TallyBuilder.cxx
  ${tarDIR}/TallyCreate.cxx
  ${tarDIR}/Tally.cxx
  ${tarDIR}/TallySelector.cxx
  ${tarDIR}/tmeshConstruct.cxx
  ${tarDIR}/tmeshTally.cxx
  ${tarINC}/cellFluxTally.h
  ${tarINC}/fissionConstruct.h
  ${tarINC}/fissionTally.h
  ${tarINC}/fluxConstruct.h
  ${tarINC}/fmeshConstruct.h
  ${tarINC}/fmeshTally.h
  ${tarINC}/gridConstruct.h
  ${tarINC}/heatConstruct.h
  ${tarINC}/heatTally.h
  ${tarINC}/itemConstruct.h
  ${tarINC}/PDControl.h
  ${tarINC}/pointConstruct.h
  ${tarINC}/pointTally.h
  ${tarINC}/sswConstruct.h
  ${tarINC}/sswTally.h
  ${tarINC}/surfaceConstruct.h
  ${tarINC}/surfaceTally.h
  ${tarINC}/TallyBuilder.h
  ${tarINC}/TallyCreate.h
  ${tarINC}/Tally.h
  ${tarINC}/TallySelector.h
  ${tarINC}/tmeshConstruct.h
  ${tarINC}/tmeshTally.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

