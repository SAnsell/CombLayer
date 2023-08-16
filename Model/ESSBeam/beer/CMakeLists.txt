set (beerSources
    BEER.cxx BEERvariables.cxx 
)

add_library (beer SHARED
  ${beerSources}
)
target_include_directories (beer PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
   ${ESS_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../beerInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BEER.cxx
  ${tarDIR}/BEERvariables.cxx
  ${tarINC}/BEER.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

