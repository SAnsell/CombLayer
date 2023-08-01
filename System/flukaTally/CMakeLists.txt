set (flukaTallySources
    flukaTallyBuilder.cxx flukaTally.cxx flukaTallyModification.cxx 
    flukaTallySelector.cxx radDecay.cxx resnucConstruct.cxx 
    resnuclei.cxx userBdxConstruct.cxx userBdx.cxx 
    userBinConstruct.cxx userBin.cxx userDumpConstruct.cxx 
    userDump.cxx userRadDecayConstruct.cxx userTrackConstruct.cxx 
    userTrack.cxx userYieldConstruct.cxx userYield.cxx 
)

add_library (flukaTally SHARED
  ${flukaTallySources}
)
target_include_directories (flukaTally PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)
file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../flukaTallyInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/flukaTallyBuilder.cxx
  ${tarDIR}/flukaTally.cxx
  ${tarDIR}/flukaTallyModification.cxx
  ${tarDIR}/flukaTallySelector.cxx
  ${tarDIR}/radDecay.cxx
  ${tarDIR}/resnucConstruct.cxx
  ${tarDIR}/resnuclei.cxx
  ${tarDIR}/userBdxConstruct.cxx
  ${tarDIR}/userBdx.cxx
  ${tarDIR}/userBinConstruct.cxx
  ${tarDIR}/userBin.cxx
  ${tarDIR}/userDumpConstruct.cxx
  ${tarDIR}/userDump.cxx
  ${tarDIR}/userRadDecayConstruct.cxx
  ${tarDIR}/userTrackConstruct.cxx
  ${tarDIR}/userTrack.cxx
  ${tarDIR}/userYieldConstruct.cxx
  ${tarDIR}/userYield.cxx
  ${tarINC}/flukaTallyBuilder.h
  ${tarINC}/flukaTally.h
  ${tarINC}/flukaTallyModification.h
  ${tarINC}/flukaTallySelector.h
  ${tarINC}/radDecay.h
  ${tarINC}/resnucConstruct.h
  ${tarINC}/resnuclei.h
  ${tarINC}/userBdxConstruct.h
  ${tarINC}/userBdx.h
  ${tarINC}/userBinConstruct.h
  ${tarINC}/userBin.h
  ${tarINC}/userDumpConstruct.h
  ${tarINC}/userDump.h
  ${tarINC}/userRadDecayConstruct.h
  ${tarINC}/userTrackConstruct.h
  ${tarINC}/userTrack.h
  ${tarINC}/userYieldConstruct.h
  ${tarINC}/userYield.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

