file(RELATIVE_PATH tarDIR "${CMAKE_BINARY_DIR}"
   "${CMAKE_CURRENT_SOURCE_DIR}")
   
add_subdirectory(cuBlock)
add_subdirectory(d4cModel)
add_subdirectory(delft)
add_subdirectory(essBuild)
add_subdirectory(essConstruct)
add_subdirectory(essLinac)
add_subdirectory(exampleBuild)
add_subdirectory(filter)
add_subdirectory(maxivBuild)
add_subdirectory(photon)
add_subdirectory(pipeBuild)
add_subdirectory(saxs)
add_subdirectory(singleItemBuild)
add_subdirectory(snsBuild)
add_subdirectory(t1Build)
add_subdirectory(t2Build)
add_subdirectory(xrayHutch)

add_subdirectory(MaxIV)
add_subdirectory(ESSBeam)
add_subdirectory(ralBuild)




set(SRC_LIST ${SRC_LIST}
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)	

