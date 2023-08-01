file(RELATIVE_PATH tarDIR "${CMAKE_BINARY_DIR}"
   "${CMAKE_CURRENT_SOURCE_DIR}")
   
add_subdirectory(beer)
add_subdirectory(bifrost)
add_subdirectory(common)
add_subdirectory(commonVar)
add_subdirectory(cspec)
add_subdirectory(dream)
add_subdirectory(estia)
add_subdirectory(freia)
add_subdirectory(heimdal)
add_subdirectory(loki)
add_subdirectory(magic)
add_subdirectory(miracles)
add_subdirectory(nmx)
add_subdirectory(nnbar)
add_subdirectory(odin)
add_subdirectory(simpleItem)
add_subdirectory(skadi)
add_subdirectory(testBeam)
add_subdirectory(trex)
add_subdirectory(vespa)
add_subdirectory(vor)

set(SRC_LIST ${SRC_LIST}
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)	

