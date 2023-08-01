set (xrayHutchSources
    DefUnitsXrayHut.cxx makeHutch.cxx xrayHutch.cxx 
    xrayHutVariables.cxx xrayTarget.cxx 
)

add_library (xrayHutch SHARED
  ${xrayHutchSources}
)
target_include_directories (xrayHutch PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${CMAKE_SOURCE_DIR}/Model/xrayHutchInc
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../xrayHutchInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DefUnitsXrayHut.cxx
  ${tarDIR}/makeHutch.cxx
  ${tarDIR}/xrayHutch.cxx
  ${tarDIR}/xrayHutVariables.cxx
  ${tarDIR}/xrayTarget.cxx
  ${tarINC}/DefUnitsXrayHut.h
  ${tarINC}/makeHutch.h
  ${tarINC}/xrayHutch.h
  ${tarINC}/xrayHutVariables.h
  ${tarINC}/xrayTarget.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

