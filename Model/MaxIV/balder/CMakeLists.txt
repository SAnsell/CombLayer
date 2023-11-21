set (balderSources
    balderConnectZone.cxx BALDER.cxx balderExptLine.cxx 
    balderFrontEnd.cxx balderOpticsLine.cxx balderVariables.cxx 
    LeadBox.cxx LeadBoxGenerator.cxx MonoVessel.cxx 
    Wiggler.cxx 
)

add_library (balder SHARED
  ${balderSources}
)
target_include_directories (balder PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../balderInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/balderConnectZone.cxx
  ${tarDIR}/BALDER.cxx
  ${tarDIR}/balderExptLine.cxx
  ${tarDIR}/balderFrontEnd.cxx
  ${tarDIR}/balderOpticsLine.cxx
  ${tarDIR}/balderVariables.cxx
  ${tarDIR}/LeadBox.cxx
  ${tarDIR}/LeadBoxGenerator.cxx
  ${tarDIR}/MonoVessel.cxx
  ${tarDIR}/Wiggler.cxx
  ${tarINC}/balderConnectZone.h
  ${tarINC}/balderExptLine.h
  ${tarINC}/balderFrontEnd.h
  ${tarINC}/BALDER.h
  ${tarINC}/balderOpticsLine.h
  ${tarINC}/LeadBoxGenerator.h
  ${tarINC}/LeadBox.h
  ${tarINC}/MonoVessel.h
  ${tarINC}/Wiggler.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

