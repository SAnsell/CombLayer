set (micromaxSources
    MICROMAX.cxx micromaxExptLineB.cxx micromaxExptLine.cxx 
    micromaxFrontEnd.cxx micromaxOpticsLine.cxx micromaxVariables.cxx 
)

add_library (micromax SHARED
  ${micromaxSources}
)
target_include_directories (micromax PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../micromaxInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/MICROMAX.cxx
  ${tarDIR}/micromaxExptLineB.cxx
  ${tarDIR}/micromaxExptLine.cxx
  ${tarDIR}/micromaxFrontEnd.cxx
  ${tarDIR}/micromaxOpticsLine.cxx
  ${tarDIR}/micromaxVariables.cxx
  ${tarINC}/micromaxExptLineB.h
  ${tarINC}/micromaxExptLine.h
  ${tarINC}/micromaxFrontEnd.h
  ${tarINC}/MICROMAX.h
  ${tarINC}/micromaxOpticsLine.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

