set (essConstructSources
    BeamShutter.cxx boltRing.cxx boxPort.cxx 
    ChopperHousing.cxx DiskBlades.cxx DiskChopper.cxx 
    InnerPort.cxx Motor.cxx SingleChopper.cxx 
    TwinBase.cxx TwinChopper.cxx TwinChopperFlat.cxx 
)

add_library (essConstruct SHARED
  ${essConstructSources}
)
target_include_directories (essConstruct PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/essConstructInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../essConstructInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BeamShutter.cxx
  ${tarDIR}/boltRing.cxx
  ${tarDIR}/boxPort.cxx
  ${tarDIR}/ChopperHousing.cxx
  ${tarDIR}/DiskBlades.cxx
  ${tarDIR}/DiskChopper.cxx
  ${tarDIR}/InnerPort.cxx
  ${tarDIR}/Motor.cxx
  ${tarDIR}/SingleChopper.cxx
  ${tarDIR}/TwinBase.cxx
  ${tarDIR}/TwinChopper.cxx
  ${tarDIR}/TwinChopperFlat.cxx
  ${tarINC}/BeamShutter.h
  ${tarINC}/boltRing.h
  ${tarINC}/boxPort.h
  ${tarINC}/ChopperHousing.h
  ${tarINC}/DiskBlades.h
  ${tarINC}/DiskChopper.h
  ${tarINC}/InnerPort.h
  ${tarINC}/Motor.h
  ${tarINC}/SingleChopper.h
  ${tarINC}/TwinBase.h
  ${tarINC}/TwinChopperFlat.h
  ${tarINC}/TwinChopper.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

