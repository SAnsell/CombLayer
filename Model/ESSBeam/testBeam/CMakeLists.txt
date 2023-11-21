set (testBeamSources
    TESTBEAM.cxx TESTBEAMvariables.cxx 
)

add_library (testBeam SHARED
  ${testBeamSources}
)
target_include_directories (testBeam PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../testBeamInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/TESTBEAM.cxx
  ${tarDIR}/TESTBEAMvariables.cxx
  ${tarINC}/TESTBEAM.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

