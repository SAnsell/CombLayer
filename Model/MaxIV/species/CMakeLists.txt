set (speciesSources
    SPECIES.cxx speciesFrontEnd.cxx speciesOpticsHut.cxx 
    speciesOpticsLine.cxx speciesVariables.cxx 
)

add_library (species SHARED
  ${speciesSources}
)
target_include_directories (species PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${MAXIV_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../speciesInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/SPECIES.cxx
  ${tarDIR}/speciesFrontEnd.cxx
  ${tarDIR}/speciesOpticsHut.cxx
  ${tarDIR}/speciesOpticsLine.cxx
  ${tarDIR}/speciesVariables.cxx
  ${tarINC}/speciesFrontEnd.h
  ${tarINC}/SPECIES.h
  ${tarINC}/speciesOpticsHut.h
  ${tarINC}/speciesOpticsLine.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

