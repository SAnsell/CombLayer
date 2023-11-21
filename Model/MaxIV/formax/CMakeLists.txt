set (formaxSources
    FORMAX.cxx formaxDetectorTube.cxx formaxExptLine.cxx 
    formaxFrontEnd.cxx formaxOpticsLine.cxx formaxVariables.cxx 
    MLMCrystal.cxx MLMonoDetail.cxx MLMRadialSupport.cxx 
    MLMSupportWheel.cxx MLMWheelPlate.cxx 
)

add_library (formax SHARED
  ${formaxSources}
)
target_include_directories (formax PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../formaxInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/FORMAX.cxx
  ${tarDIR}/formaxDetectorTube.cxx
  ${tarDIR}/formaxExptLine.cxx
  ${tarDIR}/formaxFrontEnd.cxx
  ${tarDIR}/formaxOpticsLine.cxx
  ${tarDIR}/formaxVariables.cxx
  ${tarDIR}/MLMCrystal.cxx
  ${tarDIR}/MLMonoDetail.cxx
  ${tarDIR}/MLMRadialSupport.cxx
  ${tarDIR}/MLMSupportWheel.cxx
  ${tarDIR}/MLMWheelPlate.cxx
  ${tarINC}/formaxDetectorTube.h
  ${tarINC}/formaxExptLine.h
  ${tarINC}/formaxFrontEnd.h
  ${tarINC}/FORMAX.h
  ${tarINC}/formaxOpticsLine.h
  ${tarINC}/MLMCrystal.h
  ${tarINC}/MLMonoDetail.h
  ${tarINC}/MLMRadialSupport.h
  ${tarINC}/MLMSupportWheel.h
  ${tarINC}/MLMWheelPlate.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

