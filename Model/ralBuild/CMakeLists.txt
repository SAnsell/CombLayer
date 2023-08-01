set (ralBuildSources
    BlockShutter.cxx BlockShutterGenerator.cxx BulkInsert.cxx 
    collInsert.cxx GeneralShutter.cxx ProtonVoid.cxx 
    shutterBlock.cxx TargetBase.cxx TS2ModifyTarget.cxx 
    TS2target.cxx VanePoison.cxx 
)

add_library (ralBuild SHARED
  ${ralBuildSources}
)
target_include_directories (ralBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/ralBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ralBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BlockShutter.cxx
  ${tarDIR}/BlockShutterGenerator.cxx
  ${tarDIR}/BulkInsert.cxx
  ${tarDIR}/collInsert.cxx
  ${tarDIR}/GeneralShutter.cxx
  ${tarDIR}/ProtonVoid.cxx
  ${tarDIR}/shutterBlock.cxx
  ${tarDIR}/TargetBase.cxx
  ${tarDIR}/TS2ModifyTarget.cxx
  ${tarDIR}/TS2target.cxx
  ${tarDIR}/VanePoison.cxx
  ${tarINC}/BlockShutterGenerator.h
  ${tarINC}/BlockShutter.h
  ${tarINC}/BulkInsert.h
  ${tarINC}/collInsert.h
  ${tarINC}/GeneralShutter.h
  ${tarINC}/ProtonVoid.h
  ${tarINC}/ralVariableSetup.h
  ${tarINC}/shutterBlock.h
  ${tarINC}/TargetBase.h
  ${tarINC}/TS2ModifyTarget.h
  ${tarINC}/TS2target.h
  ${tarINC}/VanePoison.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

