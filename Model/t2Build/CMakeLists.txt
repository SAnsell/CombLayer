set (t2BuildSources
    Bucket.cxx BulkShield.cxx chipDataStore.cxx 
    CoolPad.cxx CouplePipe.cxx DecFileMod.cxx 
    DecLayer.cxx Decoupled.cxx DecouplePipe.cxx 
    DPreMod.cxx GPreMod.cxx Groove.cxx 
    HPreMod.cxx HWrapper.cxx Hydrogen.cxx 
    insertInfo.cxx makeTS2.cxx OrthoInsert.cxx 
    RefBolts.cxx RefCutOut.cxx ReflectorAssembly.cxx 
    Reflector.cxx Torpedo.cxx TS2FlatTarget.cxx 
    TS2RefBolts.cxx TS2variables.cxx ts2VarModify.cxx 
    VacVessel.cxx VoidVessel.cxx 
)

add_library (t2Build SHARED
  ${t2BuildSources}
)
target_include_directories (t2Build PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/t2BuildInc
   ${CMAKE_SOURCE_DIR}/Model/ralBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../t2BuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/Bucket.cxx
  ${tarDIR}/BulkShield.cxx
  ${tarDIR}/chipDataStore.cxx
  ${tarDIR}/CoolPad.cxx
  ${tarDIR}/CouplePipe.cxx
  ${tarDIR}/DecFileMod.cxx
  ${tarDIR}/DecLayer.cxx
  ${tarDIR}/Decoupled.cxx
  ${tarDIR}/DecouplePipe.cxx
  ${tarDIR}/DPreMod.cxx
  ${tarDIR}/GPreMod.cxx
  ${tarDIR}/Groove.cxx
  ${tarDIR}/HPreMod.cxx
  ${tarDIR}/HWrapper.cxx
  ${tarDIR}/Hydrogen.cxx
  ${tarDIR}/insertInfo.cxx
  ${tarDIR}/makeTS2.cxx
  ${tarDIR}/OrthoInsert.cxx
  ${tarDIR}/RefBolts.cxx
  ${tarDIR}/RefCutOut.cxx
  ${tarDIR}/ReflectorAssembly.cxx
  ${tarDIR}/Reflector.cxx
  ${tarDIR}/Torpedo.cxx
  ${tarDIR}/TS2FlatTarget.cxx
  ${tarDIR}/TS2RefBolts.cxx
  ${tarDIR}/TS2variables.cxx
  ${tarDIR}/ts2VarModify.cxx
  ${tarDIR}/VacVessel.cxx
  ${tarDIR}/VoidVessel.cxx
  ${tarINC}/Bucket.h
  ${tarINC}/BulkShield.h
  ${tarINC}/chipDataStore.h
  ${tarINC}/CoolPad.h
  ${tarINC}/CouplePipe.h
  ${tarINC}/DecFileMod.h
  ${tarINC}/DecLayer.h
  ${tarINC}/Decoupled.h
  ${tarINC}/DecouplePipe.h
  ${tarINC}/DPreMod.h
  ${tarINC}/GPreMod.h
  ${tarINC}/Groove.h
  ${tarINC}/HPreMod.h
  ${tarINC}/HWrapper.h
  ${tarINC}/Hydrogen.h
  ${tarINC}/insertInfo.h
  ${tarINC}/makeTS2.h
  ${tarINC}/OrthoInsert.h
  ${tarINC}/RefBolts.h
  ${tarINC}/RefCutOut.h
  ${tarINC}/ReflectorAssembly.h
  ${tarINC}/Reflector.h
  ${tarINC}/Torpedo.h
  ${tarINC}/TS1build.h
  ${tarINC}/TS2FlatTarget.h
  ${tarINC}/VacVessel.h
  ${tarINC}/VoidVessel.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

