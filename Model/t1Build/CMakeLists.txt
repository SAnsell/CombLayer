set (t1BuildSources
    Cannelloni.cxx CH4Moderator.cxx CH4Pipe.cxx 
    channel.cxx H2Moderator.cxx H2Pipe.cxx 
    InnerTarget.cxx LinkWrapper.cxx LWInner.cxx 
    LWOuter.cxx makeT1Real.cxx MerlinModerator.cxx 
    MonoPlug.cxx OpenBlockTarget.cxx PlateTarget.cxx 
    PressVessel.cxx ReflectRods.cxx SideCoolTarget.cxx 
    t1BaseVariables.cxx t1BulkShield.cxx t1CylVessel.cxx 
    t1PlateTarget.cxx t1Reflector.cxx t1variables.cxx 
    t1WaterModerator.cxx WaterDividers.cxx WaterPipe.cxx 
)

add_library (t1Build SHARED
  ${t1BuildSources}
)
target_include_directories (t1Build PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/t1BuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../t1BuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Cannelloni.cxx
  ${tarDIR}/CH4Moderator.cxx
  ${tarDIR}/CH4Pipe.cxx
  ${tarDIR}/channel.cxx
  ${tarDIR}/H2Moderator.cxx
  ${tarDIR}/H2Pipe.cxx
  ${tarDIR}/InnerTarget.cxx
  ${tarDIR}/LinkWrapper.cxx
  ${tarDIR}/LWInner.cxx
  ${tarDIR}/LWOuter.cxx
  ${tarDIR}/makeT1Real.cxx
  ${tarDIR}/MerlinModerator.cxx
  ${tarDIR}/MonoPlug.cxx
  ${tarDIR}/OpenBlockTarget.cxx
  ${tarDIR}/PlateTarget.cxx
  ${tarDIR}/PressVessel.cxx
  ${tarDIR}/ReflectRods.cxx
  ${tarDIR}/SideCoolTarget.cxx
  ${tarDIR}/t1BaseVariables.cxx
  ${tarDIR}/t1BulkShield.cxx
  ${tarDIR}/t1CylVessel.cxx
  ${tarDIR}/t1PlateTarget.cxx
  ${tarDIR}/t1Reflector.cxx
  ${tarDIR}/t1variables.cxx
  ${tarDIR}/t1WaterModerator.cxx
  ${tarDIR}/WaterDividers.cxx
  ${tarDIR}/WaterPipe.cxx
  ${tarINC}/Cannelloni.h
  ${tarINC}/CH4Moderator.h
  ${tarINC}/CH4Pipe.h
  ${tarINC}/channel.h
  ${tarINC}/H2Moderator.h
  ${tarINC}/H2Pipe.h
  ${tarINC}/InnerTarget.h
  ${tarINC}/LinkWrapper.h
  ${tarINC}/LWInner.h
  ${tarINC}/LWOuter.h
  ${tarINC}/makeT1Real.h
  ${tarINC}/MerlinModerator.h
  ${tarINC}/MonoPlug.h
  ${tarINC}/OpenBlockTarget.h
  ${tarINC}/PlateTarget.h
  ${tarINC}/PressVessel.h
  ${tarINC}/RefBox.h
  ${tarINC}/ReflectRods.h
  ${tarINC}/SideCoolTarget.h
  ${tarINC}/t1BulkShield.h
  ${tarINC}/t1CylVessel.h
  ${tarINC}/t1PlateTarget.h
  ${tarINC}/t1Reflector.h
  ${tarINC}/t1WaterModerator.h
  ${tarINC}/WaterDividers.h
  ${tarINC}/WaterPipe.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

