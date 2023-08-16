set (nnbarSources
    DetectorChamber.cxx NNBAR.cxx NNBARvariables.cxx 
)

add_library (nnbar SHARED
  ${nnbarSources}
)
target_include_directories (nnbar PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nnbarInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/DetectorChamber.cxx
  ${tarDIR}/NNBAR.cxx
  ${tarDIR}/NNBARvariables.cxx
  ${tarINC}/DetectorChamber.h
  ${tarINC}/NNBAR.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

