set (vespaSources
    VESPA.cxx VespaHut.cxx VespaInner.cxx 
    VESPAvariables.cxx 
)

add_library (vespa SHARED
  ${vespaSources}
)
target_include_directories (vespa PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vespaInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/VESPA.cxx
  ${tarDIR}/VespaHut.cxx
  ${tarDIR}/VespaInner.cxx
  ${tarDIR}/VESPAvariables.cxx
  ${tarINC}/VESPA.h
  ${tarINC}/VespaHut.h
  ${tarINC}/VespaInner.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

