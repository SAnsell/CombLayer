file(RELATIVE_PATH tarDIR "${CMAKE_BINARY_DIR}"
   "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(balder)
add_subdirectory(commonBeam)
add_subdirectory(commonGenerator)
add_subdirectory(cosaxs)
add_subdirectory(danmax)
add_subdirectory(flexpes)
add_subdirectory(formax)
add_subdirectory(Linac)
add_subdirectory(maxpeem)
add_subdirectory(micromax)
add_subdirectory(R1Common)
add_subdirectory(R3Common)
add_subdirectory(softimax)
add_subdirectory(species)
add_subdirectory(GunTestFacility)

set(SRC_LIST ${SRC_LIST}
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)
