set (essLinacSources
    BeamDump.cxx DefUnitsLinac.cxx FaradayCup.cxx 
    Linac.cxx linacVariables.cxx makeLinac.cxx 
)

add_library (essLinac SHARED
  ${essLinacSources}
)
target_include_directories (essLinac PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/essLinacInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../essLinacInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BeamDump.cxx
  ${tarDIR}/DefUnitsLinac.cxx
  ${tarDIR}/FaradayCup.cxx
  ${tarDIR}/Linac.cxx
  ${tarDIR}/linacVariables.cxx
  ${tarDIR}/makeLinac.cxx
  ${tarINC}/BeamDump.h
  ${tarINC}/DefUnitsLinac.h
  ${tarINC}/FaradayCup.h
  ${tarINC}/Linac.h
  ${tarINC}/makeLinac.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

