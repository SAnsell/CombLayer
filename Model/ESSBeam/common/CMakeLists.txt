set (commonSources
    beamlineSupport.cxx EssHut.cxx 
)

add_library (common SHARED
  ${commonSources}
)
target_include_directories (common PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../commonInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/beamlineSupport.cxx
  ${tarDIR}/EssHut.cxx
  ${tarINC}/beamlineSupport.h
  ${tarINC}/EssHut.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

