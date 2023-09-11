set (softimaxSources
    BiPortTube.cxx BremOpticsColl.cxx M1BackPlate.cxx 
    M1Connectors.cxx M1Detail.cxx M1Frame.cxx M1FrontShield.cxx 
    M1Mirror.cxx M1Support.cxx SOFTIMAX.cxx 
    softimaxFrontEnd.cxx softimaxOpticsLine.cxx softimaxVariables.cxx 
)

add_library (softimax SHARED
  ${softimaxSources}
)
target_include_directories (softimax PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../softimaxInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BiPortTube.cxx
  ${tarDIR}/BremOpticsColl.cxx
  ${tarDIR}/M1BackPlate.cxx
  ${tarDIR}/M1Connectors.cxx
  ${tarDIR}/M1Detail.cxx
  ${tarDIR}/M1Frame.cxx
  ${tarDIR}/M1FrontShield.cxx
  ${tarDIR}/M1Mirror.cxx
  ${tarDIR}/M1Support.cxx
  ${tarDIR}/SOFTIMAX.cxx
  ${tarDIR}/softimaxFrontEnd.cxx
  ${tarDIR}/softimaxOpticsLine.cxx
  ${tarDIR}/softimaxVariables.cxx
  ${tarINC}/BiPortTube.h
  ${tarINC}/BremOpticsColl.h
  ${tarINC}/M1BackPlate.h
  ${tarINC}/M1Connectors.h
  ${tarINC}/M1Detail.h
  ${tarINC}/M1Frame.h
  ${tarINC}/M1FrontShield.h
  ${tarINC}/M1Mirror.h
  ${tarINC}/M1Support.h
  ${tarINC}/softimaxFrontEnd.h
  ${tarINC}/SOFTIMAX.h
  ${tarINC}/softimaxOpticsLine.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

