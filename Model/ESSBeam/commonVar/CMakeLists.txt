set (commonVarSources
    BladeGenerator.cxx ChopperGenerator.cxx CryoGenerator.cxx 
    FocusGenerator.cxx JawGenerator.cxx LayerGenerator.cxx 
    PitGenerator.cxx RotaryHoleGenerator.cxx ShieldGenerator.cxx 
    TwinBaseGenerator.cxx TwinFlatGenerator.cxx TwinGenerator.cxx 
)

add_library (commonVar SHARED
  ${commonVarSources}
)
target_include_directories (commonVar PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../commonVarInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BladeGenerator.cxx
  ${tarDIR}/ChopperGenerator.cxx
  ${tarDIR}/CryoGenerator.cxx
  ${tarDIR}/FocusGenerator.cxx
  ${tarDIR}/JawGenerator.cxx
  ${tarDIR}/LayerGenerator.cxx
  ${tarDIR}/PitGenerator.cxx
  ${tarDIR}/RotaryHoleGenerator.cxx
  ${tarDIR}/ShieldGenerator.cxx
  ${tarDIR}/TwinBaseGenerator.cxx
  ${tarDIR}/TwinFlatGenerator.cxx
  ${tarDIR}/TwinGenerator.cxx
  ${tarINC}/BladeGenerator.h
  ${tarINC}/ChopperGenerator.h
  ${tarINC}/CryoGenerator.h
  ${tarINC}/FocusGenerator.h
  ${tarINC}/JawGenerator.h
  ${tarINC}/LayerGenerator.h
  ${tarINC}/PitGenerator.h
  ${tarINC}/RotaryHoleGenerator.h
  ${tarINC}/ShieldGenerator.h
  ${tarINC}/TwinBaseGenerator.h
  ${tarINC}/TwinFlatGenerator.h
  ${tarINC}/TwinGenerator.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

