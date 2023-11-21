set (instrumentSources
    BellJar.cxx CylSample.cxx 
)

add_library (instrument SHARED
  ${instrumentSources}
)
target_include_directories (instrument PUBLIC
   ${SYSTEM_INCLUDE}
   ${GENERAL_INCLUDE}
)

file(RELATIVE_PATH tarDIR 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}")

file(RELATIVE_PATH tarINC 
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../instrumentInc")

set (SRC_LIST ${SRC_LIST}
  ${tarDIR}/BellJar.cxx
  ${tarDIR}/CylSample.cxx
  ${tarINC}/BellJar.h
  ${tarINC}/CylSample.h
  ${tarDIR}/CMakeLists.txt PARENT_SCOPE)

