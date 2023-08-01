set (exampleBuildSources
    DefUnitsExample.cxx dipoleModel.cxx dipolePipe.cxx 
    ExampleInputs.cxx exampleVariables.cxx makeExample.cxx 
    makeLinacTube.cxx quadPipe.cxx ShieldRoom.cxx 
    vacTube.cxx 
)

add_library (exampleBuild SHARED
  ${exampleBuildSources}
)
target_include_directories (exampleBuild PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/exampleBuildInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../exampleBuildInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DefUnitsExample.cxx
  ${tarDIR}/dipoleModel.cxx
  ${tarDIR}/dipolePipe.cxx
  ${tarDIR}/ExampleInputs.cxx
  ${tarDIR}/exampleVariables.cxx
  ${tarDIR}/makeExample.cxx
  ${tarDIR}/makeLinacTube.cxx
  ${tarDIR}/quadPipe.cxx
  ${tarDIR}/ShieldRoom.cxx
  ${tarDIR}/vacTube.cxx
  ${tarINC}/DefUnitsExample.h
  ${tarINC}/dipoleModel.h
  ${tarINC}/dipolePipe.h
  ${tarINC}/makeExample.h
  ${tarINC}/makeLinacTube.h
  ${tarINC}/quadPipe.h
  ${tarINC}/ShieldRoom.h
  ${tarINC}/vacTube.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

